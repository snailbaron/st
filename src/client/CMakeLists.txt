add_executable(client
    main.cpp
    timer.cpp
    view.cpp
)
target_link_libraries(client PRIVATE sdl-hpp)

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:client> $<TARGET_RUNTIME_DLLS:client>
    COMMAND_EXPAND_LISTS
)