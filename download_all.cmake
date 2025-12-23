# --- Global flags ---

# Set to TRUE if you want to wipe and rebuild everything.
if (NOT DEFINED FORCE_REBUILD)
    set(FORCE_REBUILD FALSE)
endif ()

# If TRUE: do not run smoke tests for libraries that are already installed
if (NOT DEFINED REBUILD_EXAMPLE_PROJECTS)
    set(REBUILD_EXAMPLE_PROJECTS FALSE)
endif ()

# --- Helper Methods ---

function(build_example_project LIB_NAME EXAMPLE_DIR EXAMPLE_BUILD_DIR)
    if (EXISTS "${EXAMPLE_DIR}/CMakeLists.txt")
        message(STATUS "Building example project ${LIB_NAME}...")

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

        message(STATUS "Example project build for ${LIB_NAME} PASSED.")
    else ()
        message(FATAL_ERROR "No example project found for ${LIB_NAME} (checked: ${EXAMPLE_DIR}/CMakeLists.txt)")
    endif ()
endfunction()

# Universal function to download, build, and install a library
function(download_and_install LIB_NAME LIB_URL LIB_VERSION)
    set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source/${LIB_NAME}")
    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build/${LIB_NAME}")
    set(INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install/${LIB_NAME}")
    set(EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/examples/${LIB_NAME}")
    set(EXAMPLE_BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build/examples/${LIB_NAME}")

    # 1. Clone the repository or detect VERSION_CHANGED
    set(VERSION_CHANGED FALSE)
    if (NOT EXISTS "${SOURCE_DIR}")
        # Create external_source directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")

        execute_process(
                COMMAND git clone --recursive "${LIB_URL}" "${LIB_NAME}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source"
                COMMAND_ERROR_IS_FATAL ANY
        )
    else ()
        # Check if source code already on the correct version - VERSION_CHANGED
        execute_process(
                COMMAND git rev-parse HEAD
                WORKING_DIRECTORY "${SOURCE_DIR}"
                OUTPUT_VARIABLE CURRENT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        execute_process(
                COMMAND git rev-parse "${LIB_VERSION}^{commit}"
                WORKING_DIRECTORY "${SOURCE_DIR}"
                OUTPUT_VARIABLE TARGET_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
        )

        if (NOT "${CURRENT_COMMIT_HASH}" STREQUAL "${TARGET_COMMIT_HASH}")
            set(VERSION_CHANGED TRUE)
        endif()
    endif ()


    # 2. Check if the library is already installed
    if (NOT VERSION_CHANGED AND EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
        message(STATUS "Skipping ${LIB_NAME}: Already installed in ${INSTALL_DIR}. URL: ${LIB_URL}")

        if (REBUILD_EXAMPLE_PROJECTS)
            build_example_project("${LIB_NAME}" "${EXAMPLE_DIR}" "${EXAMPLE_BUILD_DIR}")
        endif ()

        return()
    endif ()

    message(STATUS "Processing ${LIB_NAME} (${LIB_VERSION})")

    # 3. Checkout to specific commit if needed
    if (VERSION_CHANGED)
        message(STATUS "Version mismatch for ${LIB_NAME}: switching from ${CURRENT_COMMIT_HASH} to ${TARGET_COMMIT_HASH} (${LIB_VERSION})")

        # Fetch in case the tag/branch is new
        execute_process(
                COMMAND git fetch --all --tags
                WORKING_DIRECTORY "${SOURCE_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )

        # Force discard any local changes
        execute_process(
                COMMAND git reset --hard
                WORKING_DIRECTORY "${SOURCE_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )

        execute_process(
                COMMAND git checkout "${TARGET_COMMIT_HASH}"
                WORKING_DIRECTORY "${SOURCE_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )

        # Update submodules after checkout
        execute_process(
                COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY "${SOURCE_DIR}"
                COMMAND_ERROR_IS_FATAL ANY
        )
    endif ()

    # 4. Clean
    file(REMOVE_RECURSE "${BUILD_DIR}")
    file(REMOVE_RECURSE "${INSTALL_DIR}")

    # 5. Configure
    execute_process(
            COMMAND ${CMAKE_COMMAND} -S "${SOURCE_DIR}" -B "${BUILD_DIR}"
            "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}"
            "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_LIST_DIR}/external_install"
            "-DCMAKE_DEBUG_POSTFIX=d"
            ${ARGN}
            COMMAND_ERROR_IS_FATAL ANY
    )

    # 6. Build and Install Release
    message(STATUS "Installing ${LIB_NAME} [Release]...")
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}" --config Release COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND ${CMAKE_COMMAND} --install "${BUILD_DIR}" --config Release COMMAND_ERROR_IS_FATAL ANY)

    # 7. Build and Install Debug
    message(STATUS "Installing ${LIB_NAME} [Debug]...")
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}" --config Debug COMMAND_ERROR_IS_FATAL ANY)
    execute_process(COMMAND ${CMAKE_COMMAND} --install "${BUILD_DIR}" --config Debug COMMAND_ERROR_IS_FATAL ANY)

    # 8. Smoke Test: Build the example project
    build_example_project("${LIB_NAME}" "${EXAMPLE_DIR}" "${EXAMPLE_BUILD_DIR}")

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

# Record the start time (in seconds)
string(TIMESTAMP START_TIME "%s")

# --- Library List ---

download_and_install("box2d" "https://github.com/erincatto/box2d.git" "v3.1.1")
download_and_install("EnTT" "https://github.com/skypjack/entt.git" "v3.16.0" "-DENTT_INSTALL=ON")
download_and_install("nlohmann_json" "https://github.com/nlohmann/json.git" "v3.12.0" "-DJSON_BuildTests=OFF")
download_and_install("SDL3" "https://github.com/libsdl-org/SDL.git" "release-3.2.28")
# SDL3_image requires SDL3 to be installed first
download_and_install("SDL3_image" "https://github.com/libsdl-org/SDL_image.git" "release-3.2.4"
        "-DSDLIMAGE_AVIF=OFF" # fix error: No CMAKE_ASM_NASM_COMPILER could be found
)
# ImGui requires SDL3 to be installed first
download_and_install_with_custom_cmakelists("imgui" "https://github.com/ocornut/imgui.git" "v1.92.5" "imgui_CMakeLists.txt")
download_and_install("spdlog" "https://github.com/gabime/spdlog.git" "v1.16.0")
download_and_install("glm" "https://github.com/g-truc/glm.git" "1.0.2")
download_and_install("magic_enum" "https://github.com/Neargye/magic_enum.git" "v0.9.7"
        "-DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF"
        "-DMAGIC_ENUM_OPT_BUILD_TESTS=OFF"
)
download_and_install("GTest" "https://github.com/google/googletest.git" "v1.17.0")
# OpenAL is analog of SDL3_audio
download_and_install("OpenAL" "https://github.com/kcat/openal-soft.git" "1.25.0")
download_and_install_with_custom_cmakelists("implot" "https://github.com/epezent/implot.git" "v0.17" "implot_CMakeLists.txt")
# Tracy client. It sends data to Tracy server called "tracy-profiler.exe".
download_and_install("Tracy" "https://github.com/wolfpld/tracy.git" "v0.13.1")
download_and_install("miniaudio" "https://github.com/mackron/miniaudio.git" "0.11.23")
download_and_install_with_custom_cmakelists("enet" "https://github.com/lsalzman/enet.git" "v1.3.18" "enet_CMakeLists.txt")

# --- End of Library List ---

message(STATUS "download_all.cmake scripts completed with options:")
message(STATUS "  FORCE_REBUILD=${FORCE_REBUILD}")
message(STATUS "  REBUILD_EXAMPLE_PROJECTS=${REBUILD_EXAMPLE_PROJECTS}")

# Total execution time
string(TIMESTAMP END_TIME "%s")
math(EXPR DURATION "${END_TIME} - ${START_TIME}")
math(EXPR MINUTES "${DURATION} / 60")
math(EXPR SECONDS "${DURATION} % 60")
message(STATUS "Total execution time: ${MINUTES} min ${SECONDS} sec")
