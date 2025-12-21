# Global flag to control rebuild behavior.
# Set to TRUE if you want to wipe and rebuild everything.
set(FORCE_REBUILD FALSE)

# Universal function to download, build, and install a library
function(download_and_install LIB_NAME LIB_URL LIB_VERSION)
    set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source/${LIB_NAME}")
    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build/${LIB_NAME}")
    set(INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install/${LIB_NAME}")

    # Check if the library is already installed
    if(EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
        message(STATUS "--- Skipping ${LIB_NAME}: Already installed in ${INSTALL_DIR} ---")
        return()
    endif()

    message(STATUS "--- Processing ${LIB_NAME} (${LIB_VERSION}) ---")

    # 1. Clone the repository into external_source
    if(NOT EXISTS "${SOURCE_DIR}")
        # Create external_source directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")

        execute_process(
                COMMAND git clone "${LIB_URL}" "${LIB_NAME}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source"
                COMMAND_ERROR_IS_FATAL ANY
        )
    endif()

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

    message(STATUS "--- Finished ${LIB_NAME} ---")
endfunction()

# --- Function for libraries without built-in CMake (like ImGui) ---
function(download_and_install_with_custom_cmakelists LIB_NAME LIB_URL LIB_VERSION CUSTOM_CMAKE_FILE)
    set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source/${LIB_NAME}")

    # First, make sure we have the source code
    if(NOT EXISTS "${SOURCE_DIR}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")
        execute_process(COMMAND git clone "${LIB_URL}" "${LIB_NAME}" WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/external_source")
    endif()
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

# ImGui (requires SDL3 to be installed first)
# ImGui still needs special handling as it doesn't have its own CMakeLists.txt in the repo
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/external_install/SDL3")
    download_and_install_with_custom_cmakelists("imgui" "https://github.com/ocornut/imgui.git" "v1.92.5" "imgui_CMakeConfig.txt")
else()
    message(FATAL_ERROR "SDL3 installation not found at ${CMAKE_CURRENT_LIST_DIR}/external_install/SDL3. Please ensure SDL3 is listed before ImGui or already installed.")
endif()

# 2. Record the end time and calculate the duration
string(TIMESTAMP END_TIME "%s")
math(EXPR DURATION "${END_TIME} - ${START_TIME}")

# 3. Format seconds into minutes and seconds
math(EXPR MINUTES "${DURATION} / 60")
math(EXPR SECONDS "${DURATION} % 60")

message(STATUS "--------------------------------------------------")
message(STATUS "Total execution time: ${MINUTES} min ${SECONDS} sec")
message(STATUS "--------------------------------------------------")