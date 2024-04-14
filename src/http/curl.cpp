#include <http/curl.hpp>

namespace http {

Handle::Handle()
{
    _ptr.reset(checkCurl(curl_easy_init()));
}

Handle::Handle(const Handle& other)
{
    *this = other;
}

Handle::Handle(Handle&& other) noexcept
{
    auto lock = std::lock_guard(other._mutex);
    swap(*this, other);
}

Handle& Handle::operator=(const Handle& other)
{
    if (this != &other) {
        auto lock = std::lock_guard{other._mutex};
        _ptr.reset(checkCurl(curl_easy_duphandle(other._ptr.get())));
    }
    return *this;
}

Handle& Handle::operator=(Handle&& other) noexcept
{
    if (this != &other) {
        clear();
        swap(*this, other);
    }
    return *this;
}

void Handle::clear() noexcept
{
    if (_ptr) {
        _ptr.reset();
    }
}

void Handle::perform()
{
    checkCurl(curl_easy_perform(_ptr.get()));
}

void swap(Handle& lhs, Handle& rhs) noexcept
{
    std::swap(lhs._ptr, rhs._ptr);
}

} // namespace http