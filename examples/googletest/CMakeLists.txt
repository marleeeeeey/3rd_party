cmake_minimum_required(VERSION 3.20)
project(googletest_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install/googletest")

find_package(GTest REQUIRED)

add_executable(googletest_minimalProject
        main.cpp
)

target_link_libraries(googletest_minimalProject PRIVATE
        GTest::gtest_main
        GTest::gmock
)
