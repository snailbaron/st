#pragma once

#include <http/curl.hpp>

#include <map>
#include <string>

namespace http {

std::string escape(const std::string& string);

class Init {
public:
    Init();
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) = delete;
};


enum class Method {
    GET,
    POST,
};

struct Request {
    Method method = Method::GET;
    std::string url;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> headers;
};

struct Response {
    long code = 0;
    std::map<std::string, std::string> headers;
    std::string contents;
};

class Session {
public:
    Session();

    Response operator()(const Request& request);

private:
    Handle _handle;
};

} // namespace http