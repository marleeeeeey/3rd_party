cmake_minimum_required(VERSION 3.20)
project(Tracy_minimalProject)

add_executable(Tracy_minimalProject
        main.cpp
)

target_link_libraries(Tracy_minimalProject PRIVATE Tracy::TracyClient)
