add_library(fs STATIC
    fs.cpp
)
target_include_directories(fs PUBLIC include)
target_link_libraries(fs PRIVATE error)