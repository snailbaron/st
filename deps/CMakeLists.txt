add_subdirectory(Catch2)

set(CURL_ENABLE_SSL ON)
if(WIN32)
    set(CURL_USE_SCHANNEL ON)
endif()
add_subdirectory(curl)

add_subdirectory(json)