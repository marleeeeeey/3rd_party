cmake_minimum_required(VERSION 3.20)
project(asio_and_flatbuffers_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(asio REQUIRED)
find_package(flatbuffers REQUIRED)

add_subdirectory(sync_tcp)