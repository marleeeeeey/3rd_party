cmake_minimum_required(VERSION 3.20)
project(implot_minimalProject)

add_executable(implot_minimalProject
        main.cpp
)

target_link_libraries(implot_minimalProject PRIVATE implot::implot)

if (WIN32)
    add_custom_command(TARGET implot_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:implot_minimalProject>)
endif ()