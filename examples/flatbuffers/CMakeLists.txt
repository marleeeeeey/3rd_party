cmake_minimum_required(VERSION 3.20)
project(flatbuffers_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/../../external_install/${CMAKE_BUILD_TYPE}")

find_package(flatbuffers REQUIRED)

include("../../FlatbuffersHelper.cmake")

# This creates a target 'monster_schema' that you can link to anywhere
add_flatbuffers_schema(monster_schema
        monster.fbs
)

add_executable(flatbuffers_minimalProject
        main.cpp
)

target_link_libraries(flatbuffers_minimalProject PRIVATE
        monster_schema
)
