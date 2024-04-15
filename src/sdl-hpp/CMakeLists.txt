add_library(sdl-hpp STATIC
    sdl.cpp
)
target_include_directories(sdl-hpp PUBLIC include)
target_link_libraries(sdl-hpp
    PUBLIC
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf

    INTERFACE
        SDL2::SDL2main
)