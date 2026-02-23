cmake_minimum_required(VERSION 3.20)
project(flatbuffers_minimalProject)

include(${THIRD_PARTY_ROOT}/cmake/FlatbuffersHelper.cmake)

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
