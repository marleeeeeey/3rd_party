add_executable(enet_server_minimalProject
        main.cpp
)

target_link_libraries(enet_server_minimalProject PRIVATE enet::enet)
