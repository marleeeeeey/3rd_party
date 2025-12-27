cmake_minimum_required(VERSION 3.20)
project(asio_and_flatbuffers_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(asio REQUIRED)
find_package(flatbuffers REQUIRED)

include("../../FlatbuffersHelper.cmake")

add_flatbuffers_schema(solder_schema
        solder.fbs
)

add_subdirectory(server)
add_subdirectory(client)