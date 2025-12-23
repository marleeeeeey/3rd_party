cmake_minimum_required(VERSION 3.20)
project(asio_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(asio REQUIRED)

add_subdirectory(server)
add_subdirectory(client)