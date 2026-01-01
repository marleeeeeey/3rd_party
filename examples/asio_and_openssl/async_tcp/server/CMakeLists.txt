add_executable(asio_and_open_ssl_async_tcp_server_minimalProject
        main.cpp
)

target_link_libraries(asio_and_open_ssl_async_tcp_server_minimalProject PRIVATE
        asio::asio
        OpenSSL::SSL
        OpenSSL::Crypto
)