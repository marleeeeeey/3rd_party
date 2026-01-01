cmake_minimum_required(VERSION 3.20)
project(OpenAL_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install/OpenAL")

find_package(OpenAL REQUIRED)

add_executable(OpenAL_minimalProject
        main.cpp
)

target_link_libraries(OpenAL_minimalProject PRIVATE OpenAL::OpenAL)

if (WIN32)

    # HACK to determine DLL name.
    # based on the documentation from here: https://cmake.org/cmake/help/latest/module/FindOpenAL.html
    #    message(STATUS "OPENAL_LIBRARY: ${OPENAL_LIBRARY}") # C:/dev/3rd_party/external_install/OpenAL/lib/OpenAL32.lib
    string(REPLACE "/lib/" "/bin/" OPENAL_BIN_PATH "${OPENAL_LIBRARY}")
    get_filename_component(OPENAL_BIN_DIR "${OPENAL_BIN_PATH}" DIRECTORY)
    #    message(STATUS "OPENAL_BIN_DIR: ${OPENAL_BIN_DIR}") # C:/dev/3rd_party/external_install/OpenAL/bin
    set(OPENAL_DLL_PATH "${OPENAL_BIN_DIR}/OpenAL32.dll") # TODO: it always copied release DLL
    #    message(STATUS "OPENAL_DLL_PATH: ${OPENAL_DLL_PATH}") # C:/dev/3rd_party/external_install/OpenAL/bin/OpenAL32.dll

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