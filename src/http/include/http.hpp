#pragma once

#include <http/curl.hpp>

#include <nlohmann/json.hpp>

#include <map>
#include <string>

namespace http {

class CaseInsensitiveLess {
public:
    bool operator()(std::string lhs, std::string rhs) const;
};

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
    UNSET,
    GET,
    POST,
};

struct Request {
    Method method = Method::UNSET;
    std::string url;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string, CaseInsensitiveLess> headers;
    std::string data;
    nlohmann::json json;
};

struct Response {
    long code = 0;
    std::map<std::string, std::string> headers;
    std::string contents;
};

class Session {
public:
    Session();

    Response operator()(Request request);

private:
    Handle _handle;
};

} // namespace http