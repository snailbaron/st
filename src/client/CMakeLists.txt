add_executable(client
    world.cpp
    view.cpp
    timer.cpp
    main.cpp
 "geometry.cpp")
target_link_libraries(client PRIVATE imgui sdl-hpp fs http)

add_custom_command(TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:client> $<TARGET_RUNTIME_DLLS:client>
    COMMAND_EXPAND_LISTS
)