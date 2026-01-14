cmake_minimum_required(VERSION 3.20)
project(glm_minimalProject)

add_executable(glm_minimalProject
        main.cpp
)

target_link_libraries(glm_minimalProject PRIVATE glm::glm)
