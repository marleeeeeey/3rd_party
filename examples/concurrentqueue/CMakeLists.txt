cmake_minimum_required(VERSION 3.20)
project(concurrentqueue_minimalProject)

add_executable(concurrentqueue_minimalProject
        main.cpp
)

target_link_libraries(concurrentqueue_minimalProject PRIVATE
        concurrentqueue
)
