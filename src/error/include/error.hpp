#pragma once

#include <exception>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>

#ifdef __cpp_lib_stacktrace
    #include <stacktrace>
#endif

namespace e {

template <class T>
concept Streamable = requires (std::ostream& output, const T& x)
{
    output << x;
};

class Error : public std::exception {
public:
    Error(std::source_location sl = std::source_location::current())
    {
        auto stream = std::ostringstream{};
        stream <<
#ifdef __cpp_lib_stacktrace
            std::stacktrace::current() << "\n" <<
#endif
            sl.file_name() << " (" << sl.function_name() << "): " <<
            sl.line() << ":" << sl.column() << ": ";
        _message = std::move(stream).str();
    }

    const char* what() const noexcept override
    {
        return _message.c_str();
    }

    template <Streamable T>
    Error& operator<<(const T& x) &
    {
        append(x);
        return *this;
    }

    template <Streamable T>
    Error&& operator<<(const T& x) &&
    {
        append(x);
        return std::move(*this);
    }

private:
    template <Streamable T>
    void append(const T& x)
    {
        auto stream = std::ostringstream{std::move(_message), std::ios::app};
        stream << x;
        _message = std::move(stream).str();
    }

    std::string _message;
};

inline void handleError() noexcept
{
    try {
        try {
            std::rethrow_exception(std::current_exception());
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        } catch (...) {
            std::cerr << "unknown error\n";
        }
    } catch (...) { }
}

inline void require(bool condition, std::string_view message = "")
{
    if (!condition) {
        auto error = Error{};
        error << "assertion failed";
        if (!message.empty()) {
            error << ": " << message;
        }
        throw error;
    }
}

} // namespace e