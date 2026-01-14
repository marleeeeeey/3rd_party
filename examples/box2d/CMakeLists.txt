cmake_minimum_required(VERSION 3.20)
project(box2d_minimalProject)

add_executable(box2d_minimalProject
        main.cpp
)

target_link_libraries(box2d_minimalProject PRIVATE box2d::box2d)

if (WIN32)
    add_custom_command(TARGET box2d_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:box2d::box2d>
            $<TARGET_FILE_DIR:box2d_minimalProject>
            COMMENT "Copying Box2D DLL to executable directory"
    )
endif ()