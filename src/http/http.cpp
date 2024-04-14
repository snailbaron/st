#include "http.hpp"

#include <curl/curl.h>

#include <limits>
#include <ostream>
#include <sstream>
#include <utility>

namespace http {

namespace {

size_t writeData(
    void* buffer, size_t, size_t nmemb, std::ostream* output)
{
    output->write(reinterpret_cast<const char*>(buffer), nmemb);
    return nmemb;
}

size_t headerCallback(
    char* buffer,
    size_t,
    size_t nitems,
    std::map<std::string, std::string>* headers)
{
    auto string = std::string_view{buffer, nitems};
    size_t separator = string.find(":");
    if (separator == string.npos) {
        return nitems;
    }

    auto name = string.substr(0, separator);
    auto value = string.substr(separator + 1);
    headers->emplace(name, value);
    return nitems;
}

} // namespace

std::string escape(const std::string& string)
{
    e::assert(string.length() <= std::numeric_limits<int>::max());
    auto length = static_cast<int>(string.length());

    char* ptr = checkCurl(curl_easy_escape(nullptr, string.c_str(), length));
    auto result = std::string{ptr};
    curl_free(ptr);
    return result;
}

Init::Init()
{
    checkCurl(curl_global_init(CURL_GLOBAL_ALL));
}

Init::~Init()
{
    curl_global_cleanup();
}

Session::Session()
{
    _handle.setopt(CURLOPT_FOLLOWLOCATION, 1);
    _handle.setopt(CURLOPT_WRITEFUNCTION, writeData);
    _handle.setopt(CURLOPT_HEADERFUNCTION, headerCallback);
}

Response Session::operator()(const Request& request)
{
    if (request.method == Method::POST) {
        _handle.setopt(CURLOPT_POST, 1);
    }

    auto urlStream = std::ostringstream{};
    urlStream << request.url;

    bool hasWritten = false;
    for (const auto& [name, value] : request.params) {
        if (!hasWritten) {
            urlStream << "?";
            hasWritten = true;
        } else {
            urlStream << "&";
        }
        urlStream << escape(name) << "=" << escape(value);
    }

    auto urlString = urlStream.str();

    _handle.setopt(CURLOPT_URL, urlString.c_str());

    auto responseData = std::ostringstream{};
    _handle.setopt(CURLOPT_WRITEDATA, &responseData);

    auto responseHeaders = std::map<std::string, std::string>{};
    _handle.setopt(CURLOPT_HEADERDATA, &responseHeaders);

    _handle.perform();

    return Response{
        .code = _handle.getinfo<long>(CURLINFO_RESPONSE_CODE),
        .headers = std::move(responseHeaders),
        .contents = responseData.str(),
    };
}

} // namespace http