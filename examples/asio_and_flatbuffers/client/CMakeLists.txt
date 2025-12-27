add_executable(asio_and_fb_client_minimalProject
        main.cpp
)

target_link_libraries(asio_and_fb_client_minimalProject PRIVATE
        asio::asio
        solder_schema
)
