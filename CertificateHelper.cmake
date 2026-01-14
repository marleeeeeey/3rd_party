function(generate_test_certificates)

    # 1. Try to gather path from target OpenSSL::app (modern way)
    if (TARGET OpenSSL::app)
        get_target_property(OPENSSL_EXE OpenSSL::app LOCATION)
    endif ()

    # 2. If target not found or path is empty then using default variable
    if (NOT OPENSSL_EXE AND OPENSSL_EXECUTABLE)
        set(OPENSSL_EXE "${OPENSSL_EXECUTABLE}")
    endif ()

    # 3. Still not found? Try to find manually in unpack folder
    if (NOT OPENSSL_EXE AND OPENSSL_UNPACK_DIR)
        find_program(OPENSSL_EXE openssl PATHS "${OPENSSL_UNPACK_DIR}/x64/bin" NO_DEFAULT_PATH)
    endif ()

    if (NOT OPENSSL_EXE)
        message(WARNING "OpenSSL executable not found. Cannot generate certificates. (OPENSSL_UNPACK_DIR=${OPENSSL_UNPACK_DIR})")
        return()
    endif ()

    set(CERT_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    set(SERVER_CRT "${CERT_OUT_DIR}/server.crt")
    set(SERVER_KEY "${CERT_OUT_DIR}/server.key")
    set(LOCAL_OPENSSL_CONF "${CERT_OUT_DIR}/openssl-localhost.cnf")

    if (NOT EXISTS "${SERVER_CRT}" OR NOT EXISTS "${SERVER_KEY}")
        message(STATUS "Generating self-signed certificates for local testing...")

        file(MAKE_DIRECTORY "${CERT_OUT_DIR}")

        # Create config
        file(WRITE "${LOCAL_OPENSSL_CONF}" "
[ req ]
default_bits       = 2048
distinguished_name = req_distinguished_name
x509_extensions    = v3_req
prompt             = no

[ req_distinguished_name ]
CN = localhost

[ v3_req ]
keyUsage = digitalSignature, keyEncipherment
extendedKeyUsage = serverAuth
subjectAltName = @alt_names

[ alt_names ]
DNS.1 = localhost
")

        execute_process(
                COMMAND "${OPENSSL_EXE}" req -x509 -newkey rsa:2048
                -keyout "${SERVER_KEY}"
                -out "${SERVER_CRT}"
                -days 365
                -passout "pass:123Qwe!"
                -subj "/CN=localhost"
                -config "${LOCAL_OPENSSL_CONF}"
                -sha256
                RESULT_VARIABLE cert_gen_result
        )

        if (cert_gen_result EQUAL 0)
            message(STATUS "Successfully generated: ${SERVER_CRT}")
        else ()
            message(WARNING "Failed to generate certificates. Exit code: ${cert_gen_result}")
        endif ()
    endif ()
endfunction()