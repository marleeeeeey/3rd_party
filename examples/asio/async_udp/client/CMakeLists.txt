add_executable(asio_async_udp_client_minimalProject
        main.cpp
)

target_link_libraries(asio_async_udp_client_minimalProject PRIVATE asio::asio)
