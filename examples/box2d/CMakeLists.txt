cmake_minimum_required(VERSION 3.20)
project(box2d_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(box2d REQUIRED)

add_executable(box2d_minimalProject
        main.cpp
)

target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)
