cmake_minimum_required(VERSION 3.20)
project(3rd_party_examples)

add_subdirectory(box2d)
add_subdirectory(cxxopts)
add_subdirectory(enet)
add_subdirectory(EnTT)
add_subdirectory(flatbuffers)
add_subdirectory(glm)
add_subdirectory(GTest)
add_subdirectory(imgui)
add_subdirectory(implot)
add_subdirectory(magic_enum)
add_subdirectory(miniaudio)
add_subdirectory(nlohmann_json)
add_subdirectory(OpenAL)
add_subdirectory(SDL3)
add_subdirectory(SDL3_image)
add_subdirectory(spdlog)
add_subdirectory(Tracy)


add_custom_target(build_all_examples)
add_dependencies(build_all_examples
        box2d_minimalProject
        cxxopts_minimalProject
        cxxopts_dump_options_to_file_minimalProject
        enet_server_minimalProject
        enet_client_minimalProject
        EnTT_minimalProject
        flatbuffers_minimalProject
        glm_minimalProject
        GTest_minimalProject
        imgui_minimalProject
        implot_minimalProject
        magic_enum_minimalProject
        miniaudio_minimalProject
        nlohmann_json_minimalProject
        OpenAL_minimalProject
        SDL3_minimalProject
        SDL3_image_minimalProject
        spdlog_minimalProject
        Tracy_minimalProject
)


# Disable examples with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)
    add_subdirectory(asio)
    add_subdirectory(asio_and_flatbuffers)
    add_subdirectory(asio_and_openssl)
    add_subdirectory(ixwebsocket)
    add_subdirectory(uSockets)
    add_subdirectory(uWebSockets)


    add_dependencies(build_all_examples
            # Asio and OpenSSL
            asio_async_tcp_server_minimalProject
            asio_async_tcp_client_minimalProject
            asio_async_udp_server_minimalProject
            asio_async_udp_client_minimalProject
            asio_sync_tcp_server_minimalProject
            asio_sync_tcp_client_minimalProject
            asio_sync_tcp_and_fb_server_minimalProject
            asio_sync_tcp_and_fb_client_minimalProject
            asio_and_open_ssl_async_tcp_server_minimalProject
            asio_and_open_ssl_async_tcp_client_minimalProject

            # ixwebsocket
            ixwebsocket_minimalProject

            # uSockets, uWebSockets
            uSockets_tcp_no_sll_server_minimalProject
            uSockets_tcp_no_sll_client_minimalProject
            uWebSockets_HttpsListener_minimalProject
            uWebSockets_EchoServer_minimalProject
            uWebSockets_Broadcast_minimalProject
            uWebSockets_WS_BroadcastingEchoServer_minimalProject
    )
endif ()

if (EMSCRIPTEN)
    add_subdirectory(emscripten)
    add_dependencies(build_all_examples
            # Emscripten
            emscripten_basic_step01_minimalProject
            emscripten_basic_step02_minimalProject
            emscripten_basic_step03_minimalProject
            emscripten_basic_step04_minimalProject
    )
endif ()