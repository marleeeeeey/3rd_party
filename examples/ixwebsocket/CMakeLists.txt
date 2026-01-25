cmake_minimum_required(VERSION 3.20)
project(ixwebsocket_minimalProject)

add_executable(ixwebsocket_minimalProject
        main.cpp
)

target_link_libraries(ixwebsocket_minimalProject PRIVATE
        ixwebsocket::ixwebsocket
)
