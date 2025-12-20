cmake_minimum_required(VERSION 3.20)
project(SDL3_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../SDL3_installed")

find_package(SDL3 REQUIRED)

add_executable(SDL3_minimalProject
        main.cpp
)

target_link_libraries(SDL3_minimalProject PRIVATE SDL3::SDL3)

if (WIN32)
    add_custom_command(TARGET SDL3_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:SDL3_minimalProject>)
endif ()