cmake_minimum_required(VERSION 3.20)
project(asio_and_openssl_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(asio REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(async_tcp)