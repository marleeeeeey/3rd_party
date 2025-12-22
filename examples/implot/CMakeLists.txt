cmake_minimum_required(VERSION 3.20)
project(implot_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(implot REQUIRED)

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