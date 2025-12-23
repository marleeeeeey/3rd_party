add_executable(asio_server_minimalProject
        main.cpp
)

target_link_libraries(asio_server_minimalProject PRIVATE asio::asio)
