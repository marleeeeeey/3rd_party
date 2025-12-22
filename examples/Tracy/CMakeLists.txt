cmake_minimum_required(VERSION 3.20)
project(Tracy_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(Tracy REQUIRED)

add_executable(Tracy_minimalProject
        main.cpp
)

target_link_libraries(Tracy_minimalProject PRIVATE Tracy::TracyClient)
