cmake_minimum_required(VERSION 3.20)
project(uSockets_tcp_no_sll_client_minimalProject)

add_executable(uSockets_tcp_no_sll_client_minimalProject
        main.cpp
)

target_link_libraries(uSockets_tcp_no_sll_client_minimalProject PRIVATE
        uSockets::uSockets
)

add_custom_command(TARGET uSockets_tcp_no_sll_client_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_ROOT}/examples/server.crt"
        "${THIRD_PARTY_ROOT}/examples/server.key"
        $<TARGET_FILE_DIR:uSockets_tcp_no_sll_client_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)