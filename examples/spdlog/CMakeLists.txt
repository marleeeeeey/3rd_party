cmake_minimum_required(VERSION 3.20)
project(spdlog_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(spdlog REQUIRED)

add_executable(spdlog_minimalProject
        main.cpp
)

target_link_libraries(spdlog_minimalProject PRIVATE spdlog::spdlog)
