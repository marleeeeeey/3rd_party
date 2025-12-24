#include <enet/enet.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

int main(int argc, char** argv) {
  if (enet_initialize() != 0) {
    std::cerr << "Failed to initialize ENet" << std::endl;
    return EXIT_FAILURE;
  }

  atexit(enet_deinitialize);

  ENetHost* client = enet_host_create(nullptr,
                                      1,  // one outgoing connection
                                      2,  // channels
                                      0,
                                      0);

  if (!client) {
    std::cerr << "Failed to create client host" << std::endl;
    return EXIT_FAILURE;
  }

  // Initiate connection to server
  ENetAddress address;
  enet_address_set_host(&address, "127.0.0.1");
  address.port = 87654;
  ENetPeer* peer = enet_host_connect(client, &address, 2, 0);

  if (!peer) {
    std::cerr << "No available peers" << std::endl;
    return EXIT_FAILURE;
  }

  ENetEvent event;
  // Wait connection
  if (enet_host_service(client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::cout << "Connected to server" << std::endl;
  } else {
    enet_peer_reset(peer);
    std::cout << "Connection failed" << std::endl;
    return EXIT_FAILURE;
  }

  // Send a message to server
  const char* message = "Hello from client!";
  ENetPacket* packet = enet_packet_create(message,
                                          strlen(message) + 1,
                                          ENET_PACKET_FLAG_RELIABLE);

  enet_peer_send(peer, 0, packet);
  enet_host_flush(client);

  // Receive response
  while (enet_host_service(client, &event, 3000) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE) {
      std::cout << "Client received: " << event.packet->data << std::endl;
      enet_packet_destroy(event.packet);
      break;
    }
  }

  // Gracefully disconnected
  enet_peer_disconnect(peer, 0);
  while (enet_host_service(client, &event, 3000) > 0) {
    if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
      std::cout << "Disconnected from server" << std::endl;
      break;
    }
  }

  enet_host_destroy(client);
  return 0;
}
