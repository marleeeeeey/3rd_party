cmake_minimum_required(VERSION 3.20)
project(asio_and_openssl_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}/openssl/x64")

find_package(asio REQUIRED)
find_package(OpenSSL REQUIRED) # Loaded from vcpkg or conan

add_subdirectory(async_tcp)