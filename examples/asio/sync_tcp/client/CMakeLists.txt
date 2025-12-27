add_executable(asio_sync_tcp_client_minimalProject
        main.cpp
)

target_link_libraries(asio_sync_tcp_client_minimalProject PRIVATE asio::asio)
