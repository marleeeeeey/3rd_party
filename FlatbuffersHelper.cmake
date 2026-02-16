# Helper function to compile FlatBuffers schemas and create interface targets
# This file can be included in any CMake project

# Find flatc compiler target
if (NOT TARGET flatc)
    message(FATAL_ERROR "Flatbuffers target 'flatc' not found. Ensure flatbuffers is added via FetchContent.")
endif ()

# Function to compile multiple FlatBuffers schemas into a single target
# @param TARGET_NAME     Name of the library to create
# @param SCHEMAS         List of paths to .fbs schema files
function(add_flatbuffers_schema TARGET_NAME)
    # Collect all arguments after the first one as SCHEMA_PATHS
    set(SCHEMA_PATHS ${ARGN})

    # Convert relative paths to absolute paths
    set(ABSOLUTE_SCHEMA_PATHS "")
    foreach (SCHEMA_PATH IN LISTS SCHEMA_PATHS)
        if (NOT IS_ABSOLUTE "${SCHEMA_PATH}")
            list(APPEND ABSOLUTE_SCHEMA_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/${SCHEMA_PATH}")
        else ()
            list(APPEND ABSOLUTE_SCHEMA_PATHS "${SCHEMA_PATH}")
        endif ()
    endforeach ()

    # Validate each schema path
    foreach (SCHEMA_PATH IN LISTS ABSOLUTE_SCHEMA_PATHS)
        if (NOT EXISTS "${SCHEMA_PATH}")
            message(FATAL_ERROR "FlatBuffers schema not found: ${SCHEMA_PATH}")
        endif ()
    endforeach ()

    # List of all generated headers
    set(GENERATED_HEADERS "")
    set(FB_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/fb_gen") # Use binary dir to keep the source tree clean

    # Ensure the generation directory exists
    file(MAKE_DIRECTORY "${FB_GEN_DIR}")

    # Collect all output headers for tracking
    foreach (SCHEMA_PATH IN LISTS ABSOLUTE_SCHEMA_PATHS)
        get_filename_component(FILE_NAME ${SCHEMA_PATH} NAME_WE)
        set(GENERATED_HEADER "${FB_GEN_DIR}/${FILE_NAME}_generated.h")
        list(APPEND GENERATED_HEADERS "${GENERATED_HEADER}")
        message(STATUS "Flatbuffer ${FILE_NAME} output path: ${GENERATED_HEADER}:1:1")
    endforeach ()

    # Pick a host flatc when cross-compiling to Emscripten
    if (EMSCRIPTEN)
        find_program(FLATC_HOST_EXECUTABLE flatc)
        if (NOT FLATC_HOST_EXECUTABLE)
            message(FATAL_ERROR
                    "EMSCRIPTEN build requires a host 'flatc' executable in PATH. "
                    "Install flatbuffers-compiler (flatc).")
        endif ()
        set(FLATC_COMMAND "${FLATC_HOST_EXECUTABLE}")
        set(FLATC_DEPENDS ${ABSOLUTE_SCHEMA_PATHS})
    else ()
        set(FLATC_COMMAND $<TARGET_FILE:flatc>)
        set(FLATC_DEPENDS ${ABSOLUTE_SCHEMA_PATHS} flatc)
    endif ()

    # Define the generation rule (single command handles all schemas)
    add_custom_command(
            OUTPUT ${GENERATED_HEADERS}
            COMMAND "${FLATC_COMMAND}" --cpp -o "${FB_GEN_DIR}" ${ABSOLUTE_SCHEMA_PATHS}
            DEPENDS ${FLATC_DEPENDS}
            COMMENT "Compiling FlatBuffers schemas to: ${FB_GEN_DIR}"
            VERBATIM
    )

    # HACK. Part1. Generate custom target for schema files.
    set(CUSTOM_TARGET_NAME ${TARGET_NAME}_gen_task)
    add_custom_target(${CUSTOM_TARGET_NAME} DEPENDS ${GENERATED_HEADERS})

    # Create the interface library
    add_library(${TARGET_NAME} INTERFACE)

    # HACK. Part2. Make the interface library depend on the custom target
    add_dependencies(${TARGET_NAME} ${CUSTOM_TARGET_NAME})

    # Add the generated headers as sources so that CMake tracks dependencies
    target_sources(${TARGET_NAME} INTERFACE ${GENERATED_HEADERS})

    # Set include directories for the target
    target_include_directories(${TARGET_NAME} INTERFACE "${FB_GEN_DIR}")

    # Link mandatory flatbuffers headers (assumes flatbuffers target exists)
    target_link_libraries(${TARGET_NAME} INTERFACE flatbuffers)
endfunction()