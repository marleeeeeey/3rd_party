cmake_minimum_required(VERSION 3.20)
project(EnTT_minimalProject)

add_executable(EnTT_minimalProject
        main.cpp
)

target_link_libraries(EnTT_minimalProject PRIVATE EnTT::EnTT)
