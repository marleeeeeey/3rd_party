cmake_minimum_required(VERSION 3.20)
project(imgui_minimalProject)

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