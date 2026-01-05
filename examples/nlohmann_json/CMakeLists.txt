cmake_minimum_required(VERSION 3.20)
project(nlohmann_json_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(nlohmann_json REQUIRED)

add_executable(nlohmann_json_minimalProject
        main.cpp
)

target_link_libraries(nlohmann_json_minimalProject PRIVATE nlohmann_json::nlohmann_json)
