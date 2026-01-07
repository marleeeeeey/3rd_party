cmake_minimum_required(VERSION 3.20)
project(uSockets_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(uSockets REQUIRED)

add_executable(uSockets_minimalProject
        main.cpp
)

target_link_libraries(uSockets_minimalProject PRIVATE
        uSockets::uSockets
)
