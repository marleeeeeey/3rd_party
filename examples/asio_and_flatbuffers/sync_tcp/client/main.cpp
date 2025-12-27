#include <asio.hpp>
#include <iostream>
#include <thread>

#include "../NetworkMethods.h"
#include "../Serialization.h"

using asio::ip::tcp;

int main() {
  try {
    asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    asio::connect(socket, resolver.resolve("127.0.0.1", "12345"));

    std::cout << "Connected to server. Enter monster name: " << std::endl;

    std::thread reader([&socket]() {
      try {
        char reply[1024];
        while (true) {
          std::error_code ec;
          size_t length = socket.read_some(asio::buffer(reply), ec);
          if (ec)
            break;
          std::cout << "Server: " << std::string(reply, length) << std::endl;
        }
      } catch (...) {
      }
    });

    std::string line;
    while (std::getline(std::cin, line)) {
      flatbuffers::FlatBufferBuilder builder = createMonster(line);
      sendSizeAndData(socket, asio::buffer(builder.GetBufferPointer(), builder.GetSize()));
    }

    reader.join();
  } catch (std::exception& e) {
    std::cerr << "Client error: " << e.what() << std::endl;
  }
}
