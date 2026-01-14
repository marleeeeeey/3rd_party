cmake_minimum_required(VERSION 3.20)
project(uWebSockets_EchoServer_minimalProject)

add_executable(uWebSockets_EchoServer_minimalProject
        main.cpp
)

target_link_libraries(uWebSockets_EchoServer_minimalProject PRIVATE
        uWebSockets::uWebSockets
)

add_custom_command(TARGET uWebSockets_EchoServer_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../../server.crt"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../server.key"
        $<TARGET_FILE_DIR:uWebSockets_EchoServer_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)