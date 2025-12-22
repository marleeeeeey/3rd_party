# Global flag to control rebuild behavior.
# Set to TRUE if you want to wipe and rebuild everything.
set(FORCE_REBUILD FALSE)

# If TRUE: do not run smoke tests for libraries that are already installed
set(SKIP_SMOKE_TESTS_FOR_INSTALLED TRUE)

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
        if(SKIP_SMOKE_TESTS_FOR_INSTALLED)
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
                COMMAND git clone "${LIB_URL}" "${LIB_NAME}"
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
download_and_install("spdlog" "https://github.com/gabime/spdlog.git" "v1.16.0")
download_and_install("glm" "https://github.com/g-truc/glm.git" "1.0.2")
download_and_install("magic_enum" "https://github.com/Neargye/magic_enum.git" "v0.9.7"
        "-DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF"
        "-DMAGIC_ENUM_OPT_BUILD_TESTS=OFF"
)
download_and_install("googletest" "https://github.com/google/googletest.git" "v1.17.0")

# ImGui (requires SDL3 to be installed first)
# ImGui still needs special handling as it doesn't have its own CMakeLists.txt in the repo
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/external_install/SDL3")
    download_and_install_with_custom_cmakelists("imgui" "https://github.com/ocornut/imgui.git" "v1.92.5" "imgui_CMakeConfig.txt")
else ()
    message(FATAL_ERROR "SDL3 installation not found at ${CMAKE_CURRENT_LIST_DIR}/external_install/SDL3. Please ensure SDL3 is listed before ImGui or already installed.")
endif ()

# 2. Record the end time and calculate the duration
string(TIMESTAMP END_TIME "%s")
math(EXPR DURATION "${END_TIME} - ${START_TIME}")

# 3. Format seconds into minutes and seconds
math(EXPR MINUTES "${DURATION} / 60")
math(EXPR SECONDS "${DURATION} % 60")

if(SKIP_SMOKE_TESTS_FOR_INSTALLED)
    message(NOTICE "Smoke tests were skipped for libraries that were already installed (SKIP_SMOKE_TESTS_FOR_INSTALLED=ON).")
endif()

message(STATUS "--------------------------------------------------")
message(STATUS "Total execution time: ${MINUTES} min ${SECONDS} sec")
message(STATUS "--------------------------------------------------")