cmake_minimum_required(VERSION 3.20)
project(cxxopts_minimalProject)

add_executable(cxxopts_minimalProject
        main.cpp
)

target_link_libraries(cxxopts_minimalProject PRIVATE cxxopts::cxxopts)
