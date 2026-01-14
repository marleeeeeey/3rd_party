cmake_minimum_required(VERSION 3.20)
project(miniaudio_minimalProject)

add_executable(miniaudio_minimalProject
        main.cpp
)

target_link_libraries(miniaudio_minimalProject PRIVATE miniaudio)