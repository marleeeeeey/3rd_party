#include <asio.hpp>
#include <iostream>
#include <thread>

#include "solder_generated.h"  // Already includes "flatbuffers/flatbuffers.h".

using asio::ip::tcp;

int main() {
  try {
    asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve("127.0.0.1", "12345"));

    std::cout << "Connected to server. Type messages:\n";

    std::thread reader([&socket]() {
      try {
        char reply[1024];
        for (;;) {
          std::error_code ec;
          size_t length = socket.read_some(asio::buffer(reply), ec);
          if (ec) break;
          std::cout << "Server: " << std::string(reply, length) << std::endl;
        }
      } catch (...) {
      }
    });

    std::string line;
    while (std::getline(std::cin, line)) {
      asio::write(socket, asio::buffer(line));
    }

    reader.join();
  } catch (std::exception& e) {
    std::cerr << "Client error: " << e.what() << std::endl;
  }
}
