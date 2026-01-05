cmake_minimum_required(VERSION 3.20)
project(GTest_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(GTest REQUIRED)

add_executable(GTest_minimalProject
        main.cpp
)

target_link_libraries(GTest_minimalProject PRIVATE
        GTest::gtest_main
        GTest::gmock
)
