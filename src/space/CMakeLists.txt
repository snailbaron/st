add_library(space STATIC
    space.cpp
)
target_include_directories(space PUBLIC include)
target_link_libraries(space PRIVATE http)