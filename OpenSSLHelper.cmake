include(FetchContent)

# Download precompiled OpenSSL for Windows
function(install_open_ssl)

    if (WIN32)
        FetchContent_Declare(
                openssl_bin
                URL "https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-3.6.0.zip"
                URL_HASH SHA256=c1c831e8bcce7d6c204d6813aafb87c0d44dd88841ab31105185b55cdec1d759
        )
        FetchContent_MakeAvailable(openssl_bin)

        # Retrieve path to unpacked achive
        FetchContent_GetProperties(openssl_bin SOURCE_DIR OPENSSL_UNPACK_DIR)

        # Pick correct OpenSSL subfolder by platform/arch
        set(_openssl_arch_dir "")
        if (CMAKE_VS_PLATFORM_NAME STREQUAL "Win32")
            set(_openssl_arch_dir "x86")
        elseif (CMAKE_VS_PLATFORM_NAME STREQUAL "x64")
            set(_openssl_arch_dir "x64")
        elseif (CMAKE_VS_PLATFORM_NAME STREQUAL "ARM64")
            set(_openssl_arch_dir "arm64")
        elseif (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_openssl_arch_dir "x64")
        else ()
            set(_openssl_arch_dir "x86")
        endif ()

        set(OPENSSL_ROOT_DIR "${OPENSSL_UNPACK_DIR}/${_openssl_arch_dir}" CACHE PATH "OpenSSL root dir" FORCE)
        list(APPEND CMAKE_PREFIX_PATH "${OPENSSL_ROOT_DIR}")
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

    if (NOT EMSCRIPTEN)
        # Include helper and generate certificates
        include(${THIRD_PARTY_ROOT}/CertificateHelper.cmake)
        generate_test_certificates()
    endif ()

endfunction()
