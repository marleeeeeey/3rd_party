# --- Global flags ---

# Set to TRUE if you want to wipe and rebuild everything.
if (NOT DEFINED FORCE_REBUILD)
    set(FORCE_REBUILD FALSE)
endif ()

# If TRUE: do not run smoke tests for libraries that are already installed
if (NOT DEFINED BUILD_EXAMPLES)
    set(BUILD_EXAMPLES FALSE)
endif ()

set(EXTERNAL_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/external_source" CACHE PATH "Path to external libraries source code")
set(EXTERNAL_BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/external_build" CACHE PATH "Path to external libraries build artifacts")
set(EXTERNAL_INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install" CACHE PATH "Path to external libraries installation")

# --- Helper Methods ---


# CMake handles existing directories and only updates what's necessary.
function(build_cmake_project PROJECT_NAME PROJECT_SOURCE_DIR PROJECT_BUILD_DIR PROJECT_INSTALL_DIR)
    message(STATUS "Incremental build for ${PROJECT_NAME}")

    # 1. Configuration (Generation)
    execute_process(
            COMMAND ${CMAKE_COMMAND}
            -S "${PROJECT_SOURCE_DIR}"
            -B "${PROJECT_BUILD_DIR}"

            # Setup compiler
            -G Ninja
            "-DCMAKE_BUILD_TYPE=Release"
            "-DCMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO=Release"
            "-DCMAKE_C_COMPILER=clang"
            "-DCMAKE_CXX_COMPILER=clang++"

            # Other args
            "-DCMAKE_INSTALL_PREFIX=${PROJECT_INSTALL_DIR}"
            "-DCMAKE_PREFIX_PATH=${EXTERNAL_INSTALL_DIR}"
            "-DCMAKE_DEBUG_POSTFIX=d"
            ${ARGN}

            COMMAND_ERROR_IS_FATAL ANY
    )

    foreach (CONFIG IN ITEMS Debug Release)
        message(STATUS "Installing ${LIB_NAME} [${CONFIG}]...")
        execute_process(COMMAND ${CMAKE_COMMAND} --build "${PROJECT_BUILD_DIR}" --config ${CONFIG} COMMAND_ERROR_IS_FATAL ANY)
        execute_process(COMMAND ${CMAKE_COMMAND} --install "${PROJECT_BUILD_DIR}" --config ${CONFIG} COMMAND_ERROR_IS_FATAL ANY)
    endforeach ()

    message(STATUS "Project build ${PROJECT_NAME} PASSED.")
endfunction()


function(download_and_install_openssl)
    if (NOT WIN32)
        message(WARNING "Skipping OpenSSL download: This binary package is only for Windows. Please install OpenSSL manually using your system package manager.")
        return()
    endif ()

    set(OPENSSL_URL "https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-3.6.0.zip")
    set(INSTALL_DIR "${EXTERNAL_INSTALL_DIR}/openssl")
    set(TEMP_ARCHIVE "${EXTERNAL_SOURCE_DIR}/openssl.zip")

    if (EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
        message(STATUS "Skipping OpenSSL: Already installed in ${INSTALL_DIR}")
        return()
    endif ()

    message(STATUS "Downloading OpenSSL from ${OPENSSL_URL}...")
    file(DOWNLOAD "${OPENSSL_URL}" "${TEMP_ARCHIVE}" SHOW_PROGRESS STATUS DOWNLOAD_STATUS)

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if (NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download OpenSSL: ${DOWNLOAD_STATUS}")
    endif ()

    message(STATUS "Extracting OpenSSL to ${INSTALL_DIR}...")
    file(MAKE_DIRECTORY "${INSTALL_DIR}")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${TEMP_ARCHIVE}"
            WORKING_DIRECTORY "${INSTALL_DIR}"
            COMMAND_ERROR_IS_FATAL ANY
    )

    file(REMOVE "${TEMP_ARCHIVE}")
    message(STATUS "Finished OpenSSL")
endfunction()


# CMake download, build, and install a library
function(download_and_install LIB_NAME LIB_URL LIB_VERSION CUSTOM_CMAKE_FILE)
    set(SOURCE_DIR "${EXTERNAL_SOURCE_DIR}/${LIB_NAME}")
    set(BUILD_DIR "${EXTERNAL_BUILD_DIR}/${LIB_NAME}")
    set(INSTALL_DIR "${EXTERNAL_INSTALL_DIR}/${LIB_NAME}")

    # 1. Clone the repository or detect VERSION_CHANGED
    set(VERSION_CHANGED FALSE)
    if (NOT EXISTS "${SOURCE_DIR}")
        # Create external_source directory if it doesn't exist
        file(MAKE_DIRECTORY "${EXTERNAL_SOURCE_DIR}")

        execute_process(
                COMMAND git clone --recursive "${LIB_URL}" "${LIB_NAME}"
                WORKING_DIRECTORY "${EXTERNAL_SOURCE_DIR}"
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
        endif ()
    endif ()


#    # 2. Check if the library is already installed
#    if (NOT VERSION_CHANGED AND EXISTS "${INSTALL_DIR}" AND NOT FORCE_REBUILD)
#        message(STATUS "Skipping ${LIB_NAME}: Already installed in ${INSTALL_DIR}. URL: ${LIB_URL}")
#        return()
#    endif ()

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

    # 3.1 Patching with custom CMakeLists.txt if provided
    set(PATCHED_CMAKE FALSE)
    if (CUSTOM_CMAKE_FILE)
        message(STATUS "Patching ${LIB_NAME} with custom file: ${CUSTOM_CMAKE_FILE}")
        file(COPY "${CMAKE_CURRENT_LIST_DIR}/${CUSTOM_CMAKE_FILE}" DESTINATION "${SOURCE_DIR}")
        file(RENAME "${SOURCE_DIR}/${CUSTOM_CMAKE_FILE}" "${SOURCE_DIR}/CMakeLists.txt")
        set(PATCHED_CMAKE TRUE)
    endif ()

    # 4. Clean
    if (FORCE_REBUILD)
        file(REMOVE_RECURSE "${BUILD_DIR}")
        file(REMOVE_RECURSE "${INSTALL_DIR}")
    endif ()

    build_cmake_project(${LIB_NAME} ${SOURCE_DIR} ${BUILD_DIR} ${INSTALL_DIR} ${ARGN})

    message(STATUS "Finished ${LIB_NAME}")
endfunction()


function(run_all_downloads)
    download_and_install_openssl()
    download_and_install("box2d" "https://github.com/erincatto/box2d.git" "v3.1.1" "")
    download_and_install("EnTT" "https://github.com/skypjack/entt.git" "v3.16.0" ""
            "-DENTT_INSTALL=ON")
    download_and_install("nlohmann_json" "https://github.com/nlohmann/json.git" "v3.12.0" ""
            "-DJSON_BuildTests=OFF")
    download_and_install("SDL3" "https://github.com/libsdl-org/SDL.git" "release-3.2.28" "")
    # SDL3_image requires SDL3 to be installed first
    download_and_install("SDL3_image" "https://github.com/libsdl-org/SDL_image.git" "release-3.2.4" ""
            "-DSDLIMAGE_AVIF=OFF" # fix error: No CMAKE_ASM_NASM_COMPILER could be found
    )
    # ImGui requires SDL3 to be installed first
    download_and_install("imgui" "https://github.com/ocornut/imgui.git" "v1.92.5" "imgui_CMakeLists.txt")
    download_and_install("spdlog" "https://github.com/gabime/spdlog.git" "v1.16.0" "")
    download_and_install("glm" "https://github.com/g-truc/glm.git" "1.0.2" "")
    download_and_install("magic_enum" "https://github.com/Neargye/magic_enum.git" "v0.9.7" ""
            "-DMAGIC_ENUM_OPT_BUILD_EXAMPLES=OFF"
            "-DMAGIC_ENUM_OPT_BUILD_TESTS=OFF"
    )
    download_and_install("GTest" "https://github.com/google/googletest.git" "v1.17.0" "")
    # OpenAL is analog of SDL3_audio
    download_and_install("OpenAL" "https://github.com/kcat/openal-soft.git" "1.25.0" "")
    download_and_install("implot" "https://github.com/epezent/implot.git" "v0.17" "implot_CMakeLists.txt")
    # Tracy client. It sends data to Tracy server called "tracy-profiler.exe".
    download_and_install("Tracy" "https://github.com/wolfpld/tracy.git" "v0.13.1" ""
            "-DCMAKE_CXX_STANDARD=20")
    download_and_install("miniaudio" "https://github.com/mackron/miniaudio.git" "0.11.23" "")
    download_and_install("enet" "https://github.com/lsalzman/enet.git" "v1.3.18" "enet_CMakeLists.txt")
    download_and_install("asio" "https://github.com/chriskohlhoff/asio.git" "asio-1-36-0" "asio_CMakeLists.txt")
    download_and_install("flatbuffers" "https://github.com/google/flatbuffers.git" "v25.12.19" ""
            "-DFLATBUFFERS_BUILD_TESTS=OFF")
    download_and_install("cxxopts" "https://github.com/jarro2783/cxxopts.git" "v3.3.1" ""
            "-DCXXOPTS_BUILD_TESTS=OFF")
endfunction()


function(main)
    # Record the start time (in seconds)
    string(TIMESTAMP START_TIME "%s")

    # Handle complex dependencies via vcpkg
    # setup_vcpkg_and_install_manifest()

    run_all_downloads()

    # --- Smoke Test: Build all examples ---
    if (BUILD_EXAMPLES)
        message(STATUS "Installing examples")
        set(EXAMPLES_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/examples")
        set(EXAMPLES_BUILD_DIR "${EXTERNAL_BUILD_DIR}/examples")
        set(EXAMPLES_INSTALL_DIR "${EXTERNAL_INSTALL_DIR}/examples")
        build_cmake_project("examples" "${EXAMPLES_SOURCE_DIR}" "${EXAMPLES_BUILD_DIR}" "${EXAMPLES_INSTALL_DIR}")
    endif ()

    message(STATUS "download_all.cmake scripts completed with options:")
    message(STATUS "  FORCE_REBUILD=${FORCE_REBUILD}")
    message(STATUS "  BUILD_EXAMPLES=${BUILD_EXAMPLES}")

    # Total execution time
    string(TIMESTAMP END_TIME "%s")
    math(EXPR DURATION "${END_TIME} - ${START_TIME}")
    math(EXPR MINUTES "${DURATION} / 60")
    math(EXPR SECONDS "${DURATION} % 60")
    message(STATUS "Total execution time: ${MINUTES} min ${SECONDS} sec")
endfunction()


# --- MAIN ---

if (CMAKE_SCRIPT_MODE_FILE)
    main()
endif ()
