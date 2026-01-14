cmake_minimum_required(VERSION 3.20)
project(spdlog_minimalProject)

add_executable(spdlog_minimalProject
        main.cpp
)

target_link_libraries(spdlog_minimalProject PRIVATE spdlog::spdlog)
