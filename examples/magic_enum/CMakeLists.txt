cmake_minimum_required(VERSION 3.20)
project(magic_enum_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(magic_enum REQUIRED)

add_executable(magic_enum_minimalProject
        main.cpp
)

target_link_libraries(magic_enum_minimalProject PRIVATE magic_enum::magic_enum)
