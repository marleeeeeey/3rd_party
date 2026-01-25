cmake_minimum_required(VERSION 3.20)
project(ixwebsocket_minimalProject)

add_executable(ixwebsocket_minimalProject
        main.cpp
)

target_link_libraries(ixwebsocket_minimalProject PRIVATE
        ixwebsocket::ixwebsocket
)

add_custom_command(TARGET ixwebsocket_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_ROOT}/examples/server.crt"
        "${THIRD_PARTY_ROOT}/examples/server.key"
        $<TARGET_FILE_DIR:ixwebsocket_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)