#include "http.hpp"

#include <curl/curl.h>

#include <cctype>
#include <algorithm>
#include <format>
#include <limits>
#include <ostream>
#include <sstream>
#include <utility>

namespace http {

namespace {

size_t readDataCallback(
    char* buffer, size_t, size_t nitems, std::istream* input)
{
    input->read(buffer, nitems);
    return input->gcount();
}

size_t writeDataCallback(
    void* buffer, size_t, size_t nmemb, std::ostream* output)
{
    output->write(reinterpret_cast<const char*>(buffer), nmemb);
    return nmemb;
}

size_t writeHeadersCallback(
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

void polishRequestInPlace(Request& request)
{
    const bool hasData = !request.data.empty();
    const bool hasJson = !request.json.empty();

    if (hasData && hasJson) {
        throw e::Error{} << "cannot set both .data and .json in http::Request";
    }

    if (request.method == Method::UNSET) {
        if (hasData || hasJson) {
            request.method = Method::POST;
        } else {
            request.method = Method::GET;
        }
    }

    if (!request.headers.contains("Content-Type")) {
        if (hasJson) {
            request.headers.emplace("Content-Type", "application/json");
        }
    }
}

} // namespace

bool CaseInsensitiveLess::operator()(
    std::string lhs, std::string rhs) const
{
    auto charToLower = [] (unsigned char c) {
        return static_cast<char>(std::tolower(c));
    };

    std::ranges::transform(lhs, lhs.begin(), charToLower);
    std::ranges::transform(rhs, rhs.begin(), charToLower);
    return lhs < rhs;
}

std::string escape(const std::string& string)
{
    e::require(string.length() <= std::numeric_limits<int>::max());
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

nlohmann::json Response::json() const
{
    return nlohmann::json::parse(contents);
}

Session::Session()
{
    _handle.setopt(CURLOPT_FOLLOWLOCATION, 1);
    _handle.setopt(CURLOPT_READFUNCTION, readDataCallback);
    _handle.setopt(CURLOPT_WRITEFUNCTION, writeDataCallback);
    _handle.setopt(CURLOPT_HEADERFUNCTION, writeHeadersCallback);
}

Response Session::operator()(Request request)
{
    polishRequestInPlace(request);

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

    auto headerStringList = StringList{};
    for (const auto& [name, value] : request.headers) {
        headerStringList.append(std::format("{}: {}", name, value));
    }
    _handle.setopt(CURLOPT_HTTPHEADER, headerStringList.ptr());

    auto dataInput = std::istringstream{};
    auto dataBuffer = std::string{};
    if (!request.data.empty()) {
        dataInput = std::istringstream{request.data};
    } else if (!request.json.empty()) {
        dataBuffer = request.json.dump();
        dataInput = std::istringstream{dataBuffer};
    }
    dataInput.exceptions(std::ios::badbit);
    _handle.setopt(CURLOPT_READDATA, &dataInput);

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