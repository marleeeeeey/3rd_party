cmake_minimum_required(VERSION 3.20)
project(nlohmann_json_minimalProject)

add_executable(nlohmann_json_minimalProject
        main.cpp
)

target_link_libraries(nlohmann_json_minimalProject PRIVATE nlohmann_json::nlohmann_json)
