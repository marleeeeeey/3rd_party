cmake_minimum_required(VERSION 3.20)
project(uSockets_tcp_no_sll_server_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(uSockets REQUIRED)

add_executable(uSockets_tcp_no_sll_server_minimalProject
        main.cpp
)

target_link_libraries(uSockets_tcp_no_sll_server_minimalProject PRIVATE
        uSockets::uSockets
)

add_custom_command(TARGET uSockets_tcp_no_sll_server_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../server.crt"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../server.key"
        $<TARGET_FILE_DIR:uSockets_tcp_no_sll_server_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)