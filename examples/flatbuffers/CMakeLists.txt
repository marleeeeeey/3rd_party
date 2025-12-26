cmake_minimum_required(VERSION 3.20)
project(flatbuffers_minimalProject)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "../../external_install")

find_package(flatbuffers REQUIRED)

# Set path to the flatc compiler
set(FLATC_PATH "${CMAKE_SOURCE_DIR}/../external_install/flatbuffers/bin")
find_program(FLATC_EXECUTABLE flatc PATHS ${FLATC_PATH})
if (NOT FLATC_EXECUTABLE)
    message(FATAL_ERROR "flatc compiler not found! Please build flatbuffers first.")
endif ()

# Directory where all generated FlatBuffers headers will be placed
set(FB_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# Function to compile FlatBuffers schema (.fbs)
# SCHEMA_FILE - Path to the input .fbs file
# OUT_VAR     - Name of the variable to store the path to the generated .h file
function(compile_flatbuffers SCHEMA_FILE OUT_VAR)
    get_filename_component(SCHEMA_NAME ${SCHEMA_FILE} NAME_WE)

    # Place generated files into a dedicated directory in the build folder
    set(GEN_HEADER "${FB_GEN_DIR}/${SCHEMA_NAME}_generated.h")

    # Ensure the output directory exists
    file(MAKE_DIRECTORY "${FB_GEN_DIR}")

    # Define the generation rule
    add_custom_command(
            OUTPUT "${GEN_HEADER}"
            COMMAND ${FLATC_EXECUTABLE} --cpp -o "${FB_GEN_DIR}" "${SCHEMA_FILE}"
            DEPENDS "${SCHEMA_FILE}"
            COMMENT "Generating FlatBuffers header for ${SCHEMA_FILE}"
            VERBATIM
    )

    # Export the generated header path to the parent scope
    set(${OUT_VAR} "${GEN_HEADER}" PARENT_SCOPE)
endfunction()

# Generate the header for monster.fbs
compile_flatbuffers("${CMAKE_CURRENT_SOURCE_DIR}/monster.fbs" MONSTER_H)

add_executable(flatbuffers_minimalProject
        main.cpp
        "${MONSTER_H}" # Include in sources so CMake knows it's a dependency
)

target_link_libraries(flatbuffers_minimalProject PRIVATE
        flatbuffers::flatbuffers)

target_include_directories(flatbuffers_minimalProject PRIVATE
        "${FB_GEN_DIR}"
)