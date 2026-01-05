cmake_minimum_required(VERSION 3.20)
project(imgui_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(imgui REQUIRED)

add_executable(imgui_minimalProject
        main.cpp
)

target_link_libraries(imgui_minimalProject PRIVATE imgui::imgui)

if (WIN32)
    add_custom_command(TARGET imgui_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:imgui_minimalProject>)
endif ()