cmake_minimum_required(VERSION 3.20)
project(miniaudio_minimalProject)

set(CMAKE_CXX_STANDARD 17)

set(MINIAUDIO_INCLUDE_DIR "../../external_install/${CMAKE_BUILD_TYPE}/miniaudio/include")

add_executable(miniaudio_minimalProject
        main.cpp
)

target_include_directories(miniaudio_minimalProject PRIVATE ${MINIAUDIO_INCLUDE_DIR})