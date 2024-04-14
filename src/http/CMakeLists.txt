add_library(http
    curl.cpp
    http.cpp
)
target_include_directories(http PUBLIC include)
target_link_libraries(http
    PUBLIC CURL::libcurl error
)