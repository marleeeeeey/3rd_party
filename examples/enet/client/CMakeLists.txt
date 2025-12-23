add_executable(enet_client_minimalProject
        main.cpp
)

target_link_libraries(enet_client_minimalProject PRIVATE enet::enet)
