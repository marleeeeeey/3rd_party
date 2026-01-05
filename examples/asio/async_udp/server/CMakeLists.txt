add_executable(asio_async_udp_server_minimalProject
        main.cpp
)

target_link_libraries(asio_async_udp_server_minimalProject PRIVATE asio::asio)
