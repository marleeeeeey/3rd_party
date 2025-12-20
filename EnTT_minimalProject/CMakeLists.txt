cmake_minimum_required(VERSION 3.20)
project(EnTT_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../EnTT_installed")

find_package(EnTT REQUIRED)

add_executable(EnTT_minimalProject
        main.cpp
)

target_link_libraries(EnTT_minimalProject PRIVATE EnTT::EnTT)
