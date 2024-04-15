set(CURL_ENABLE_SSL ON)
if(WIN32)
    set(CURL_USE_SCHANNEL ON)
endif()

add_subdirectory(Catch2)
add_subdirectory(curl)
add_subdirectory(json)
add_subdirectory(SDL)
add_subdirectory(SDL_image)
add_subdirectory(SDL_mixer)
add_subdirectory(SDL_ttf)

# TODO: imgui