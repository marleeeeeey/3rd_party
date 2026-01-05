cmake_minimum_required(VERSION 3.20)
project(glm_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(glm REQUIRED)

add_executable(glm_minimalProject
        main.cpp
)

target_link_libraries(glm_minimalProject PRIVATE glm::glm)
