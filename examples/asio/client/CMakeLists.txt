add_executable(asio_client_minimalProject
        main.cpp
)

target_link_libraries(asio_client_minimalProject PRIVATE asio::asio)
