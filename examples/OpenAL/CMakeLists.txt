cmake_minimum_required(VERSION 3.20)
project(OpenAL_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}/OpenAL")

find_package(OpenAL REQUIRED)

add_executable(OpenAL_minimalProject
        main.cpp
)

target_link_libraries(OpenAL_minimalProject PRIVATE OpenAL::OpenAL)

if (WIN32)

    # Use the variable from download_all.cmake for path consistency
    set(OPENAL_DLL_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}/OpenAL/bin/OpenAL32.dll")

    if (NOT EXISTS "${OPENAL_DLL_PATH}")
        message(FATAL_ERROR "OPENAL_DLL_PATH=${OPENAL_DLL_PATH} is not found. Check if EXTERNAL_INSTALL_DIR is correct.")
    endif()

    if (EXISTS "${OPENAL_DLL_PATH}")
        add_custom_command(TARGET OpenAL_minimalProject POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENAL_DLL_PATH}
                $<TARGET_FILE_DIR:OpenAL_minimalProject>
        )
    else ()
        message(FATAL_ERROR "OPENAL_DLL_PATH=${OPENAL_DLL_PATH} is not found.")
    endif ()

endif ()