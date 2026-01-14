cmake_minimum_required(VERSION 3.20)
project(magic_enum_minimalProject)

add_executable(magic_enum_minimalProject
        main.cpp
)

target_link_libraries(magic_enum_minimalProject PRIVATE magic_enum::magic_enum)
