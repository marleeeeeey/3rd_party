cmake_minimum_required(VERSION 3.20)
project(uWebSockets_Broadcast_minimalProject)

add_executable(uWebSockets_Broadcast_minimalProject
        main.cpp
)

target_link_libraries(uWebSockets_Broadcast_minimalProject PRIVATE
        uWebSockets::uWebSockets
)

add_custom_command(TARGET uWebSockets_Broadcast_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${THIRD_PARTY_ROOT}/examples/server.crt"
        "${THIRD_PARTY_ROOT}/examples/server.key"
        $<TARGET_FILE_DIR:uWebSockets_Broadcast_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)