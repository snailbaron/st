add_executable(client
    main.cpp
    protocol.cpp
    resources.cpp
    timer.cpp
    view.cpp
    widgets.cpp
    world.cpp
 )
target_link_libraries(client PRIVATE sdl-hpp fs http)

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:client> $<TARGET_RUNTIME_DLLS:client>
    COMMAND_EXPAND_LISTS
)

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:client>/assets
)