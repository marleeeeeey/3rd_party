#include <enet/enet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

int main( int argc, char** argv )
{
    if ( enet_initialize() != 0 )
    {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return EXIT_FAILURE;
    }

    atexit( enet_deinitialize );

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 87654;

    ENetHost* server = enet_host_create(
                                        &address,
                                        32, // max clients
                                        2, // channels
                                        0,
                                        0
                                       );

    if ( !server )
    {
        std::cerr << "Failed to create server host" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Server started on port " << address.port << std::endl;

    ENetEvent event;

    while ( true )
    {
        while ( enet_host_service( server, &event, 1000 ) > 0 )
        {
            switch ( event.type )
            {
            case ENET_EVENT_TYPE_CONNECT:
                {
                    std::cout << "Client connected from "
                        << event.peer->address.host << ":"
                        << event.peer->address.port << std::endl;
                    event.peer->data = (void*)"Client";
                    break;
                }

            case ENET_EVENT_TYPE_RECEIVE:
                {
                    std::cout << "Server received: " << event.packet->data << std::endl;

                    const char* reply = "Hello from server!";
                    ENetPacket* packet = enet_packet_create(
                                                            reply,
                                                            strlen( reply ) + 1,
                                                            ENET_PACKET_FLAG_RELIABLE
                                                           );
                    enet_peer_send( event.peer, 0, packet );

                    enet_packet_destroy( event.packet );
                    break;
                }

            case ENET_EVENT_TYPE_DISCONNECT:
                {
                    std::cout << "Client disconnected" << std::endl;
                    event.peer->data = nullptr;
                    break;
                }

            default:
                {
                    break;
                }
            }
        }
    }

    enet_host_destroy( server );
    return 0;
}
