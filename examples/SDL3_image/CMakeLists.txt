cmake_minimum_required(VERSION 3.20)
project(SDL3_image_minimalProject)

add_executable(SDL3_image_minimalProject
        main.cpp
)

target_link_libraries(SDL3_image_minimalProject PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
)

if (WIN32)
    add_custom_command(TARGET SDL3_image_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:SDL3_image_minimalProject>
    )
endif ()