cmake_minimum_required(VERSION 3.20)
project(cxxopts_dump_options_to_file_minimalProject)

add_executable(cxxopts_dump_options_to_file_minimalProject
        main.cpp
)

target_link_libraries(cxxopts_dump_options_to_file_minimalProject PRIVATE
        cxxopts::cxxopts
)
