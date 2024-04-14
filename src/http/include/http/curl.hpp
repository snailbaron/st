#pragma once

#include "error.hpp"

#include <curl/curl.h>

#include <mutex>

namespace http {

inline void checkCurl(CURLcode code)
{
    if (code != CURLE_OK) {
        throw e::Error{} << "CURL error: " << code;
    }
}

template <class T>
T* checkCurl(T* ptr)
{
    if (ptr == nullptr) {
        throw e::Error{} << "CURL error";
    }
    return ptr;
}

class Handle {
public:
    Handle();
    Handle(const Handle& other);
    Handle(Handle&& other) noexcept;

    Handle& operator=(const Handle& other);
    Handle& operator=(Handle&& other) noexcept;

    void clear() noexcept;
    void perform();

    template <class T>
    void setopt(CURLoption option, T&& value)
    {
        checkCurl(curl_easy_setopt(_ptr.get(), option, std::forward<T>(value)));
    }

    template <class T>
    T getinfo(CURLINFO info)
    {
        auto value = T{};
        checkCurl(curl_easy_getinfo(_ptr.get(), info, &value));
        return value;
    }

    friend void swap(Handle& lhs, Handle& rhs) noexcept;

private:
    std::unique_ptr<CURL, void(*)(CURL*)> _ptr{nullptr, curl_easy_cleanup};
    mutable std::mutex _mutex;
};

} // namespace http