#include "http.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

using json = nlohmann::json;

int main() try
{
    auto init = http::Init{};

    auto session = http::Session{};

    auto response = session(http::Request{
        .url = "https://echo.free.beeceptor.com",
        .params = {
            {"one", "two"},
            {"three", "four"},
        },
        .headers = {
            {"Something-Something", "This is a header"},
            {"Another-Something", "This is another header"},
        },
        .json = {
            {"some key", 12},
            {"another key", "abcde"},
            {"some list", {1, 2, 3, 4, 6}},
            {"list of structs", {
                {
                    {"one", "two"},
                },
                {
                    {"three", "four"},
                }
            }}
        },
    });

    std::cout << "response code: " << response.code << "\n";
    std::cout << "contents:\n" << response.contents << "\n";
} catch (...) {
    e::handleError();
    return EXIT_FAILURE;
}