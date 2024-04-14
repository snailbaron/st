add_executable(terminal
    main.cpp
)
target_link_libraries(terminal
    PRIVATE nlohmann_json::nlohmann_json http arg space
)

add_custom_command(TARGET terminal POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:terminal> $<TARGET_RUNTIME_DLLS:terminal>
    COMMAND_EXPAND_LISTS
)