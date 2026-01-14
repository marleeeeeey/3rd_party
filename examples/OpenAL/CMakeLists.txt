cmake_minimum_required(VERSION 3.20)
project(OpenAL_minimalProject)

add_executable(OpenAL_minimalProject
        main.cpp
)

target_link_libraries(OpenAL_minimalProject PRIVATE OpenAL::OpenAL)

if (WIN32)
    add_custom_command(TARGET OpenAL_minimalProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:OpenAL::OpenAL>
            $<TARGET_FILE_DIR:OpenAL_minimalProject>
            COMMENT "Copying OpenAL DLL to executable directory"
    )
endif ()