add_library(http STATIC
    curl.cpp
    http.cpp
)
target_include_directories(http PUBLIC include)
target_link_libraries(http
    PUBLIC CURL::libcurl nlohmann_json::nlohmann_json error
)