# --- Config ---

# 1. Set build type if not present
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT CMAKE_GENERATOR)
    find_program(NINJA_PATH ninja)
    if(NINJA_PATH)
        set(CMAKE_GENERATOR "Ninja")
        set(CMAKE_MAKE_PROGRAM "${NINJA_PATH}")
    elseif(WIN32)
        set(CMAKE_GENERATOR "Visual Studio 17 2022")
    else()
        set(CMAKE_GENERATOR "Unix Makefiles")
    endif()
endif()

if(NOT CMAKE_C_COMPILER)
    find_program(C_COMPILER_PATH NAMES clang gcc cl cc)
    if(C_COMPILER_PATH)
        set(CMAKE_C_COMPILER "${C_COMPILER_PATH}")
    endif()
endif()

if(NOT CMAKE_CXX_COMPILER)
    find_program(CXX_COMPILER_PATH NAMES clang++ g++ cl CC)
    if(CXX_COMPILER_PATH)
        set(CMAKE_CXX_COMPILER "${CXX_COMPILER_PATH}")
    endif()
endif()

# 2. Set paths
set(EXTERNAL_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/external_source")
set(EXTERNAL_BUILD_DIR   "${CMAKE_CURRENT_LIST_DIR}/external_build/${CMAKE_BUILD_TYPE}")
set(EXTERNAL_INSTALL_DIR "${CMAKE_CURRENT_LIST_DIR}/external_install/${CMAKE_BUILD_TYPE}")

# 3. Custom flags
set(FORCE_REBUILD  FALSE CACHE BOOL "Wipe and rebuild everything")
set(BUILD_EXAMPLES TRUE CACHE BOOL "Build smoke tests/examples")


# --- Helper Methods ---


function(ExternalDependencies_print_global_variables)
    message(STATUS "------------------------------------------------------------")
    message(STATUS "3rd Party Projects Configuration:")
    message(STATUS "  Generator:       ${CMAKE_GENERATOR}")
    message(STATUS "  Build Type:      ${CMAKE_BUILD_TYPE}")
    message(STATUS "  Make Program:    ${CMAKE_MAKE_PROGRAM}")
    message(STATUS "  C Compiler:      ${CMAKE_C_COMPILER}")
    message(STATUS "  CXX Compiler:    ${CMAKE_CXX_COMPILER}")
    message(STATUS "  Source Dir:      ${EXTERNAL_SOURCE_DIR}")
    message(STATUS "  Build Dir:       ${EXTERNAL_BUILD_DIR}")
    message(STATUS "  Install Dir:     ${EXTERNAL_INSTALL_DIR}")
    message(STATUS "  Force Rebuild:   ${FORCE_REBUILD}")
    message(STATUS "  Build Examples:  ${BUILD_EXAMPLES}")
    message(STATUS "------------------------------------------------------------")
endfunction()


# CMake handles existing directories and only updates what's necessary.
function(build_cmake_project PROJECT_NAME PROJECT_SOURCE_DIR PROJECT_BUILD_DIR PROJECT_INSTALL_DIR)
    message("")
    message("")
    message(STATUS "Incremental build for ${PROJECT_NAME}")

    set(COMMON_ARGS
            "-G${CMAKE_GENERATOR}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_INSTALL_PREFIX=${PROJECT_INSTALL_DIR}"
            "-DCMAKE_PREFIX_PATH=${EXTERNAL_INSTALL_DIR}"
            "-DCMAKE_CXX_STANDARD=17"
            "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
    )

    # Next arguments added if not present. Sometimes it may help to autodetect them
    if(CMAKE_C_COMPILER)
        list(APPEND COMMON_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
    endif()
    if(CMAKE_CXX_COMPILER)
        list(APPEND COMMON_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
    endif()
    if(CMAKE_MAKE_PROGRAM)
        list(APPEND COMMON_ARGS "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}")
    endif()

    # Hack to proceed OpenSSL path to uWebSocket libraries to help find_package()
    set(OPENSSL_ROOT "${EXTERNAL_INSTALL_DIR}/openssl/x64")
    if(EXISTS "${OPENSSL_ROOT}")
        list(APPEND COMMON_ARGS "-DOPENSSL_ROOT_DIR=${OPENSSL_ROOT}")
    endif()

    message(STATUS "Final arguments for ${PROJECT_NAME}:")
    foreach(arg IN LISTS COMMON_ARGS ARGN)
        message(STATUS "  ${arg}")
    endforeach()

    message(STATUS "CMAKE_PREFIX_PATH for ${PROJECT_NAME}:")
    foreach(path IN LISTS CMAKE_PREFIX_PATH)
        message(STATUS "  ${path}")
    endforeach()


    message(STATUS "Configuring ${PROJECT_NAME} [${CMAKE_BUILD_TYPE}]...")
    execute_process(
            COMMAND ${CMAKE_COMMAND}
            -S "${PROJECT_SOURCE_DIR}"
            -B "${PROJECT_BUILD_DIR}"
            ${COMMON_ARGS}
            ${ARGN}
            COMMAND_ERROR_IS_FATAL ANY
    )

    message(STATUS "Building ${PROJECT_NAME} [${CMAKE_BUILD_TYPE}]...")
    execute_process(
            COMMAND ${CMAKE_COMMAND}
            --build "${PROJECT_BUILD_DIR}"
            --config ${CMAKE_BUILD_TYPE}
            COMMAND_ERROR_IS_FATAL ANY
    )

    message(STATUS "Installing ${PROJECT_NAME} [${CMAKE_BUILD_TYPE}]...")
    execute_process(
            COMMAND ${CMAKE_COMMAND}
            --install "${PROJECT_BUILD_DIR}"
            --config ${CMAKE_BUILD_TYPE}
            COMMAND_ERROR_IS_FATAL ANY
    )

    message(STATUS "Project build ${PROJECT_NAME} PASSED.")
endfunction()


function(generate_test_certificates)
    set(OPENSSL_EXE "${CMAKE_CURRENT_SOURCE_DIR}/external_install/${CMAKE_BUILD_TYPE}/openssl/x64/bin/openssl.exe")
    set(OPENSSL_CONF "${CMAKE_CURRENT_SOURCE_DIR}/external_install/${CMAKE_BUILD_TYPE}/openssl/ssl/openssl.cnf")
    set(CERT_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    set(SERVER_CRT "${CERT_OUT_DIR}/server.crt")
    set(SERVER_KEY "${CERT_OUT_DIR}/server.key")

    if (NOT EXISTS "${SERVER_CRT}" OR NOT EXISTS "${SERVER_KEY}")
        message(STATUS "Generating self-signed certificates for local testing...")

        file(MAKE_DIRECTORY "${CERT_OUT_DIR}")

        execute_process(
                COMMAND "${OPENSSL_EXE}" req -x509 -newkey rsa:2048
                -keyout "${SERVER_KEY}"
                -out "${SERVER_CRT}"
                -days 365
                -passout "pass:123Qwe!"
                -subj "/CN=localhost"
                -config "${OPENSSL_CONF}"
                RESULT_VARIABLE cert_gen_result
                COMMAND_ERROR_IS_FATAL ANY
        )

        if (cert_gen_result EQUAL 0)
            message(STATUS "Successfully generated: ${SERVER_CRT}")
        else ()
            message(WARNING "Failed to generate certificates. OpenSSL exit code: ${cert_gen_result}")
        endif ()
    else ()
        message(STATUS "Certificates already exist in ${CERT_OUT_DIR}, skipping generation.")
    endif ()
endfunction()


function(download_and_install_openssl)
    message("")
    message("")
    message(STATUS "Download OpenSSL as achieve and unpack")
    if (NOT WIN32)
        message(WARNING "Skipping OpenSSL download: This binary package is only for Windows. Please install OpenSSL manually using your system package manager.")
        return()
    endif ()

    set(OPENSSL_URL "https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-3.6.0.zip")
    set(EXPECTED_OPENSSL_SHA256 "c1c831e8bcce7d6c204d6813aafb87c0d44dd88841ab31105185b55cdec1d759")
    set(INSTALL_DIR "${EXTERNAL_INSTALL_DIR}/openssl")
    set(TEMP_ARCHIVE "${EXTERNAL_SOURCE_DIR}/openssl.zip")

    set(NEED_DOWNLOAD TRUE)
    if (EXISTS "${TEMP_ARCHIVE}")
        file(SHA256 "${TEMP_ARCHIVE}" ACTUAL_OPENSSL_SHA256)
        message(STATUS "ACTUAL_OPENSSL_SHA256=${ACTUAL_OPENSSL_SHA256}")
        if ("${ACTUAL_OPENSSL_SHA256}" STREQUAL "${EXPECTED_OPENSSL_SHA256}")
            message(STATUS "OpenSSL archive already exists and hash is valid. Skipping download.")
            set(NEED_DOWNLOAD FALSE)
        else ()
            message(WARNING "OpenSSL archive hash mismatch. Re-downloading...")
            file(REMOVE "${TEMP_ARCHIVE}")
        endif ()
    endif ()

    if (NEED_DOWNLOAD)
        message(STATUS "Downloading OpenSSL from ${OPENSSL_URL}...")
        file(DOWNLOAD "${OPENSSL_URL}" "${TEMP_ARCHIVE}"
                EXPECTED_HASH SHA256=${EXPECTED_OPENSSL_SHA256}
                SHOW_PROGRESS
                STATUS DOWNLOAD_STATUS)

        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

        if (NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download OpenSSL: DOWNLOAD_STATUS=${DOWNLOAD_STATUS}. ERROR_MESSAGE=${ERROR_MESSAGE}")
        endif ()
    endif ()

    message(STATUS "Extracting OpenSSL to ${INSTALL_DIR}...")
    file(MAKE_DIRECTORY "${INSTALL_DIR}")
    execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf "${TEMP_ARCHIVE}"
            WORKING_DIRECTORY "${INSTALL_DIR}"
            COMMAND_ERROR_IS_FATAL ANY
    )

    message(STATUS "Finished OpenSSL extraction")
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


function(ExternalDependencies_download_all)
    download_and_install_openssl()
    generate_test_certificates()
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
    download_and_install("OpenAL" "https://github.com/kcat/openal-soft.git" "1.25.0" ""
            "-DCMAKE_CXX_SCAN_FOR_MODULES=OFF"
            "-DALSOFT_EXAMPLES=OFF"
            "-DALSOFT_TESTS=OFF"
    )
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
    download_and_install("libuv" "https://github.com/libuv/libuv.git" "v1.51.0" ""
            "-DLIBUV_BUILD_TESTS=OFF"
            "-DLIBUV_BUILD_SHARED=OFF")
    download_and_install("uSockets" "https://github.com/uNetworking/uSockets.git" "182b7e4fe7211f98682772be3df89c71dc4884fa" "uSockets_CMakeLists.txt")
    download_and_install("uWebSockets" "https://github.com/uNetworking/uWebSockets.git" "v20.74.0" "uWebSockets_CMakeLists.txt")

    if (BUILD_EXAMPLES)
        message(STATUS "Installing examples")
        set(EXAMPLES_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/examples")
        set(EXAMPLES_BUILD_DIR "${EXTERNAL_BUILD_DIR}/examples")
        set(EXAMPLES_INSTALL_DIR "${EXTERNAL_INSTALL_DIR}/examples")
        build_cmake_project("examples" "${EXAMPLES_SOURCE_DIR}" "${EXAMPLES_BUILD_DIR}" "${EXAMPLES_INSTALL_DIR}")
    endif ()
endfunction()
