cmake_minimum_required(VERSION 3.20)
project(uWebSockets_Broadcast_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(uWebSockets REQUIRED)

add_executable(uWebSockets_Broadcast_minimalProject
        main.cpp
)

target_link_libraries(uWebSockets_Broadcast_minimalProject PRIVATE
        uWebSockets::uWebSockets
)

add_custom_command(TARGET uWebSockets_Broadcast_minimalProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/../../server.crt"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../server.key"
        $<TARGET_FILE_DIR:uWebSockets_Broadcast_minimalProject>
        COMMENT "IMPORTANT: Copying tests SSL certificates to output directory"
)