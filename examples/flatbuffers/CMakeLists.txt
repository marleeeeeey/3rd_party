cmake_minimum_required(VERSION 3.20)
project(flatbuffers_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(flatbuffers REQUIRED)

#
# Generate header file for serialization using flatc.exe
#
set(FBS_SCHEMA "${CMAKE_CURRENT_SOURCE_DIR}/monster.fbs")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers_gen")
set(GENERATED_HEADER "${GENERATED_DIR}/monster_generated.h")
find_program(FLATC_EXECUTABLE flatc PATHS "${CMAKE_SOURCE_DIR}/../external_install/flatbuffers/bin")
if(NOT FLATC_EXECUTABLE)
    message(FATAL_ERROR "flatc compiler not found! Please build flatbuffers first.")
endif()
add_custom_command(
        OUTPUT "${GENERATED_HEADER}"
        COMMAND ${FLATC_EXECUTABLE} --cpp -o "${GENERATED_DIR}" "${FBS_SCHEMA}"
        DEPENDS "${FBS_SCHEMA}"
        COMMENT "Generating FlatBuffers header for ${FBS_SCHEMA} to ${GENERATED_HEADER}"
)

add_executable(flatbuffers_minimalProject
        main.cpp
        "${GENERATED_HEADER}"
)

target_link_libraries(flatbuffers_minimalProject PRIVATE
        flatbuffers::flatbuffers)
