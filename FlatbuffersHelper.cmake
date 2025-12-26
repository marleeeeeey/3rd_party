# Helper function to compile FlatBuffers schemas and create interface targets
# This file can be included in any CMake project

# Find flatc compiler if not already found
if (NOT FLATC_EXECUTABLE)
    set(FLATC_PATH "${CMAKE_SOURCE_DIR}/external_install/flatbuffers/bin")
    find_program(FLATC_EXECUTABLE flatc PATHS ${FLATC_PATH})
endif ()

# Function to compile FlatBuffers schema and create a logical target
# @param TARGET_NAME  Name of the library to create
# @param SCHEMA_PATH  Path to the .fbs schema file (can be relative to current CMakeLists.txt)
function(add_flatbuffers_schema TARGET_NAME SCHEMA_PATH)
    # Convert relative path to absolute relative to the calling CMakeLists.txt
    if (NOT IS_ABSOLUTE "${SCHEMA_PATH}")
        set(SCHEMA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_PATH}")
    endif ()

    if (NOT EXISTS "${SCHEMA_PATH}")
        message(FATAL_ERROR "FlatBuffers schema not found: ${SCHEMA_PATH}")
    endif ()

    get_filename_component(FILE_NAME ${SCHEMA_PATH} NAME_WE)

    # Use binary directory to keep source tree clean
    set(FB_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_fb")
    set(GENERATED_HEADER "${FB_GEN_DIR}/${FILE_NAME}_generated.h")

    # Ensure the generation directory exists
    file(MAKE_DIRECTORY "${FB_GEN_DIR}")

    # Define the generation rule
    add_custom_command(
            OUTPUT "${GENERATED_HEADER}"
            COMMAND ${FLATC_EXECUTABLE} --cpp -o "${FB_GEN_DIR}" "${SCHEMA_PATH}"
            DEPENDS "${SCHEMA_PATH}"
            COMMENT "Compiling FlatBuffers schema: ${SCHEMA_PATH}"
            VERBATIM
    )

    # Create the interface library
    add_library(${TARGET_NAME} INTERFACE)

    # Add the generated header as a source so CMake tracks dependencies
    target_sources(${TARGET_NAME} INTERFACE "${GENERATED_HEADER}")

    # Set include directories for the target
    target_include_directories(${TARGET_NAME} INTERFACE "${FB_GEN_DIR}")

    # Link mandatory flatbuffers headers (assumes flatbuffers::flatbuffers target exists)
    target_link_libraries(${TARGET_NAME} INTERFACE flatbuffers::flatbuffers)
endfunction()