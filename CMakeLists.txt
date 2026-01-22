cmake_minimum_required(VERSION 3.24)

# We are using obsolete method FetchContent_Populate to patch repositories without CMakeLists.txt.
if (POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif ()

project(3rd_party_libraries C CXX)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23 CACHE STRING "C++ standard to use")
endif ()

# ---------------------------------------------------

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared globally for WASM mostly")

# ---------------------------------------------------

include(FetchContent)

# --- 0. Download precompiled OpenSSL for Windows ---

if (WIN32)
    FetchContent_Declare(
            openssl_bin
            URL "https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-3.6.0.zip"
            URL_HASH SHA256=c1c831e8bcce7d6c204d6813aafb87c0d44dd88841ab31105185b55cdec1d759
    )
    FetchContent_MakeAvailable(openssl_bin)

    # Retrieve path to unpacked achive
    FetchContent_GetProperties(openssl_bin SOURCE_DIR OPENSSL_UNPACK_DIR)

    # Add x64 to the path for Windows binaries
    list(APPEND CMAKE_PREFIX_PATH "${OPENSSL_UNPACK_DIR}/x64")
endif ()

if (NOT EMSCRIPTEN)
    # This will find system OpenSSL on Linux (libssl-dev) and the downloaded one on Windows
    find_package(OpenSSL REQUIRED)
    message(STATUS "OpenSSL found at: ${OPENSSL_INCLUDE_DIR}")
else ()
    message(STATUS "Emscripten detected: skipping OpenSSL search (using browser capabilities)")
endif ()

# --- Install rules for precompiled OpenSSL ---
if (WIN32)
    # Install Headers
    install(DIRECTORY "${OPENSSL_INCLUDE_DIR}/openssl"
            DESTINATION include
            FILES_MATCHING PATTERN "*.h")

    # Install Libraries (.lib files for linking)
    install(FILES "${OPENSSL_CRYPTO_LIBRARY}" "${OPENSSL_SSL_LIBRARY}"
            DESTINATION lib)

    # Install DLLs (if they exist in the same bin folder as the zip structure)
    # Usually they are in the 'bin' or 'x64' folder of the unpacked archive
    get_filename_component(OPENSSL_BIN_DIR "${OPENSSL_INCLUDE_DIR}/../bin" ABSOLUTE)
    if (EXISTS "${OPENSSL_BIN_DIR}")
        install(DIRECTORY "${OPENSSL_BIN_DIR}/"
                DESTINATION bin
                FILES_MATCHING PATTERN "*.dll")
    endif ()
endif ()

# Include helper and generate certificates
include(CertificateHelper.cmake)
generate_test_certificates()

# --- 1. Standard libs (FetchContent_MakeAvailable) ---

FetchContent_Declare(box2d GIT_REPOSITORY https://github.com/erincatto/box2d.git GIT_TAG v3.1.1)

FetchContent_Declare(entt GIT_REPOSITORY https://github.com/skypjack/entt.git GIT_TAG v3.16.0)
set(ENTT_INSTALL ON CACHE BOOL "")

FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.12.0)
set(JSON_BuildTests OFF CACHE BOOL "")

FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-3.2.28)

FetchContent_Declare(SDL3_image GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git GIT_TAG release-3.2.4)
set(SDLIMAGE_AVIF OFF CACHE BOOL "")

FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.16.0)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.2)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "")
set(GLM_BUILD_TESTS OFF CACHE BOOL "")
set(GLM_INSTALL OFF CACHE BOOL "")

FetchContent_Declare(magic_enum GIT_REPOSITORY https://github.com/Neargye/magic_enum.git GIT_TAG v0.9.7)
set(MAGIC_ENUM_OPT_BUILD_EXAMPLES OFF CACHE BOOL "")
set(MAGIC_ENUM_OPT_BUILD_TESTS OFF CACHE BOOL "")

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.17.0)
set(gtest_force_shared_crt ON CACHE BOOL "Prevent overriding the parent project's settings (Windows)")

FetchContent_Declare(openal-soft GIT_REPOSITORY https://github.com/kcat/openal-soft.git GIT_TAG 1.25.0)
set(LIBTYPE STATIC CACHE STRING "")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "")
set(ALSOFT_TESTS OFF CACHE BOOL "")
set(ALSOFT_EMBED_HRTF_DATA ON CACHE BOOL "")

FetchContent_Declare(tracy GIT_REPOSITORY https://github.com/wolfpld/tracy.git GIT_TAG v0.13.1)

FetchContent_Declare(flatbuffers GIT_REPOSITORY https://github.com/google/flatbuffers.git GIT_TAG v25.12.19)
set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "")

FetchContent_Declare(cxxopts GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git GIT_TAG v3.3.1)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "")

if (NOT EMSCRIPTEN)
    FetchContent_Declare(libuv GIT_REPOSITORY https://github.com/libuv/libuv.git GIT_TAG v1.51.0)
    set(LIBUV_BUILD_TESTS OFF CACHE BOOL "")
    set(LIBUV_BUILD_SHARED ON CACHE BOOL "")
endif ()

# Make standard libs available. They are not required any patches

set(LIBS_TO_POPULATE
        box2d entt nlohmann_json SDL3 SDL3_image spdlog
        glm magic_enum googletest openal-soft tracy
        flatbuffers cxxopts
)

if (NOT EMSCRIPTEN)
    list(APPEND LIBS_TO_POPULATE libuv)
endif ()

FetchContent_MakeAvailable(${LIBS_TO_POPULATE})

# --- 2. Libs with custom CMakeLists.txt ---

# Declare them
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.92.5)
FetchContent_Declare(implot GIT_REPOSITORY https://github.com/epezent/implot.git GIT_TAG v0.17)
FetchContent_Declare(enet GIT_REPOSITORY https://github.com/lsalzman/enet.git GIT_TAG v1.3.18)
FetchContent_Declare(asio GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git GIT_TAG asio-1-36-0)
FetchContent_Declare(uSockets GIT_REPOSITORY https://github.com/uNetworking/uSockets.git GIT_TAG 182b7e4fe7211f98682772be3df89c71dc4884fa)
FetchContent_Declare(uWebSockets GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git GIT_TAG v20.74.0)

set(PATCHED_LIBS imgui implot enet asio)

# Disable uSockets and uWebSockets builds because Emscripten don't need them.
# Emscripten contains own Emscripten WebSockets API.
# https://emscripten.org/docs/porting/networking.html
if (NOT EMSCRIPTEN)
    list(APPEND PATCHED_LIBS uSockets uWebSockets)
endif ()

# Patching loop
foreach (lib IN LISTS PATCHED_LIBS)
    # Convert to lower because FetchContent saves props and pathes in lower case
    string(TOLOWER "${lib}" lib_lc)

    FetchContent_GetProperties(${lib_lc})
    if (NOT ${lib_lc}_POPULATED)
        message(STATUS "Populating ${lib} (internal name: ${lib_lc})...")

        FetchContent_Populate(${lib_lc})

        set(SRC_DIR "${${lib_lc}_SOURCE_DIR}")
        set(BIN_DIR "${${lib_lc}_BINARY_DIR}")

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_${lib}.txt")
            message(STATUS "Patching ${lib} in ${SRC_DIR}")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists_${lib}.txt"
                    "${SRC_DIR}/CMakeLists.txt" COPYONLY)
        endif ()

        if (SRC_DIR)
            add_subdirectory("${SRC_DIR}" "${BIN_DIR}")
        else ()
            message(FATAL_ERROR "Could not find source directory for ${lib}. Check FetchContent_Declare name.")
        endif ()
    endif ()
endforeach ()

# --- 3. Header-only without CMake (miniaudio) ---

FetchContent_Declare(miniaudio GIT_REPOSITORY https://github.com/mackron/miniaudio.git GIT_TAG 0.11.23)
# Stop CMake from glaze into miniaudio CMakeLists and using this library as Header-Only
FetchContent_GetProperties(miniaudio)
if (NOT miniaudio_POPULATED)
    FetchContent_Populate(miniaudio)
    if (NOT TARGET miniaudio)
        add_library(miniaudio INTERFACE)
        target_include_directories(miniaudio INTERFACE ${miniaudio_SOURCE_DIR})
    endif ()
endif ()

# --- Add examples ---
set(3RD_PARTY_BUILD_EXAMPLES OFF CACHE BOOL "Build integrated examples. It produce a lot of CMake targets of example projects")
if (3RD_PARTY_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
    add_subdirectory(examples)
endif ()