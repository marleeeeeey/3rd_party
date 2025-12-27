add_executable(asio_and_fb_server_minimalProject
        main.cpp
        ../Serialization.cpp
        ../NetworkMethods.cpp
)

target_link_libraries(asio_and_fb_server_minimalProject PRIVATE
        asio::asio
        solder_schema
)
