cmake_minimum_required(VERSION 3.20)
project(GTest_minimalProject)

add_executable(GTest_minimalProject
        main.cpp
)

target_link_libraries(GTest_minimalProject PRIVATE
        GTest::gtest_main
        GTest::gmock
        GTest::gtest
)
