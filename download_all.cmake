# Global flag to control rebuild behavior.
# Set to TRUE if you want to wipe and rebuild everything.
if(NOT DEFINED FORCE_REBUILD)
    set(FORCE_REBUILD FALSE)
endif()

# If TRUE: do not run smoke tests for libraries that are already installed
if(NOT DEFINED REBUILD_EXAMPLE_PROJECTS)
    set(REBUILD_EXAMPLE_PROJECTS FALSE)
endif()

function(run_smoke_test LIB_NAME EXAMPLE_DIR EXAMPLE_BUILD_DIR)
    if (EXISTS "${EXAMPLE_DIR}/CMakeLists.txt")
        message(STATUS "Running Smoke Test for ${LIB_NAME}...")

        file(REMOVE_RECURSE "${EXAMPLE_BUILD_DIR}")

        execute_process(
                COMMAND ${CMAKE_COMMAND} -S "${EXAMPLE_DIR}" -B "${EXAMPLE_BUILD_DIR}"
                "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_LIST_DIR}/external_install"
                COMMAND_ERROR_IS_FATAL ANY
        )

        execute_process(
                COMMAND ${CMAKE_COMMAND} --build "${EXAMPLE_BUILD_DIR}" --config Release
                COMMAND_ERROR_IS_FATAL ANY
        )

        execute_process(
                COMMAND ${CMAKE_COMMAND} --build "${EXAMPLE_BUILD_DIR}" --config Debug
                COMMAND_ERROR_IS_FATAL ANY
        )

        message(STATUS "Smoke Test for ${LIB_NAME} PASSED.")
    else ()
        message(FATAL_ERROR "No smoke test found for ${LIB_NAME} (checked: ${EXAMPLE_DIR}/CMakeLists.txt)")
    endif ()
endfunction()

# Universal function to download, build, and install a library
function(download_and_install LIB_NAME LIB_URL LIB_VERSION)
    set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source/${LIB_NAME}")
    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build/${LIB_NAME}")
    set(INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install/${LIB_NAME}")
    set(EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/examples/${LIB_NAME}")
    set(EXAMPLE_BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build/examples/${LIB_NAME}")

    # Check if the library is already installed
    if (EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
        message(STATUS "Skipping ${LIB_NAME}: Already installed in ${INSTALL_DIR}")

        # Optionally also skip smoke tests for installed libs
        if(NOT REBUILD_EXAMPLE_PROJECTS)
            return()
        endif()

        # Otherwise run smoke test even for installed libs
        run_smoke_test("${LIB_NAME}" "${EXAMPLE_DIR}" "${EXAMPLE_BUILD_DIR}")
        return()
    endif ()

    message(STATUS "Processing ${LIB_NAME} (${LIB_VERSION})")

    # 1. Clone the repository into external_source
    if (NOT EXISTS "${SOURCE_DIR}")
        # Create external_source directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")

        execute_process(
                COMMAND git clone --recursive "${LIB_URL}" "${LIB_NAME}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source"
                COMMAND_ERROR_IS_FATAL ANY
        )
    endif ()

    # 2. Checkout the specified version/tag
    execute_process(
            COMMAND git checkout "${LIB_VERSION}"
            WORKING_DIRECTORY "${SOURCE_DIR}"
            COMMAND_ERROR_IS_FATAL ANY
    )

    # 3. Clean
    file(REMOVE_RECURSE "${BUILD_DIR}")
    file(REMOVE_RECURSE "${INSTALL_DIR}")

    # 4. Configure
    # Note: Removed quotes from the value part of -D flags because CMake handles them
    execute_process(
            COMMAND ${CMAKE_COMMAND} -S "${SOURCE_DIR}" -B "${BUILD_DIR}"
            "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
            # Add our central install directory to the prefix path so dependencies can be found
            "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_LIST_DIR}/external_install"
            -DCMAKE_DEBUG_POSTFIX=d
            ${ARGN}
            COMMAND_ERROR_IS_FATAL ANY
    )

    # 5. Build and Install Release
    message(STATUS "Installing ${LIB_NAME} [Release]...")
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}" --config Release COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND ${CMAKE_COMMAND} --install "${BUILD_DIR}" --config Release COMMAND_ERROR_IS_FATAL ANY)

    # 6. Build and Install Debug
    message(STATUS "Installing ${LIB_NAME} [Debug]...")
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}" --config Debug COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND ${CMAKE_COMMAND} --install "${BUILD_DIR}" --config Debug COMMAND_ERROR_IS_FATAL ANY)

    # 7. Smoke Test: Build the example project
    run_smoke_test("${LIB_NAME}" "${EXAMPLE_DIR}" "${EXAMPLE_BUILD_DIR}")

    message(STATUS "Finished ${LIB_NAME}")
endfunction()

# Function for libraries without built-in CMake (like ImGui)
function(download_and_install_with_custom_cmakelists LIB_NAME LIB_URL LIB_VERSION CUSTOM_CMAKE_FILE)
    set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source/${LIB_NAME}")
    set(INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install/${LIB_NAME}")

    # Check if the library is already installed
    if (EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
        message(STATUS "Skipping ${LIB_NAME}: Already installed in ${INSTALL_DIR}")
        return()
    endif ()

    # First, make sure we have the source code
    if (NOT EXISTS "${SOURCE_DIR}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")
        execute_process(COMMAND git clone "${LIB_URL}" "${LIB_NAME}" WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")
    endif ()
    execute_process(COMMAND git checkout "${LIB_VERSION}" WORKING_DIRECTORY "${SOURCE_DIR}")

    # Copy the custom CMakeLists.txt into the library source folder
    message(STATUS "Patching ${LIB_NAME} with ${CUSTOM_CMAKE_FILE}...")
    file(COPY "${CMAKE_CURRENT_LIST_DIR}/${CUSTOM_CMAKE_FILE}" DESTINATION "${SOURCE_DIR}")
    file(RENAME "${SOURCE_DIR}/${CUSTOM_CMAKE_FILE}" "${SOURCE_DIR}/CMakeLists.txt")

    # Call the standard build function (now that CMakeLists.txt is present)
    download_and_install("${LIB_NAME}" "${LIB_URL}" "${LIB_VERSION}" ${ARGN})

    # Cleanup: remove the injected CMakeLists.txt
    file(REMOVE "${SOURCE_DIR}/CMakeLists.txt")
endfunction()

# --- MAIN ---

# 1. Record the start time (in seconds)
string(TIMESTAMP START_TIME "%s")

# --- Library List ---

download_and_install("box2d" "https://github.com/erincatto/box2d.git" "v3.1.1")
download_and_install("EnTT" "https://github.com/skypjack/entt.git" "v3.16.0" "-DENTT_INSTALL=ON")
download_and_install("nlohmann_json" "https://github.com/nlohmann/json.git" "v3.12.0" "-DJSON_BuildTests=OFF")
download_and_install("SDL3" "https://github.com/libsdl-org/SDL.git" "release-3.2.28")
# ImGui requires SDL3 to be installed first
download_and_install_with_custom_cmakelists("imgui" "https://github.com/ocornut/imgui.git" "v1.92.5" "imgui_CMakeLists.txt")
download_and_install("spdlog" "https://github.com/gabime/spdlog.git" "v1.16.0")
download_and_install("glm" "https://github.com/g-truc/glm.git" "1.0.2")
download_and_install("magic_enum" "https://github.com/Neargye/magic_enum.git" "v0.9.7"
        "-DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF"
        "-DMAGIC_ENUM_OPT_BUILD_TESTS=OFF"
)
download_and_install("GTest" "https://github.com/google/googletest.git" "v1.17.0")
download_and_install("SDL3_image" "https://github.com/libsdl-org/SDL_image.git" "release-3.2.4"
        "-DSDLIMAGE_AVIF=OFF" # fix error: No CMAKE_ASM_NASM_COMPILER could be found
)

# 2. Record the end time and calculate the duration
string(TIMESTAMP END_TIME "%s")
math(EXPR DURATION "${END_TIME} - ${START_TIME}")

# 3. Format seconds into minutes and seconds
math(EXPR MINUTES "${DURATION} / 60")
math(EXPR SECONDS "${DURATION} % 60")

message(STATUS "download_all.cmake scripts completed with options:")
message(STATUS "  FORCE_REBUILD=${FORCE_REBUILD}")
message(STATUS "  REBUILD_EXAMPLE_PROJECTS=${REBUILD_EXAMPLE_PROJECTS}")

message(STATUS "Total execution time: ${MINUTES} min ${SECONDS} sec")
