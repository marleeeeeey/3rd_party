#include <asio.hpp>
#include <iostream>
#include <memory>

#include "../NetworkMethods.h"
#include "../Serialization.h"

using asio::ip::tcp;

// Session with one client
void session(std::shared_ptr<tcp::socket> clientSocketPrt) {
  std::cout << "New session with client. "
            << "Client address: " << clientSocketPrt->remote_endpoint().address().to_string()
            << std::endl;

  try {
    while (true) {
      // Read monster data from the client
      std::vector<uint8_t> monsterData = readSizeAndData(clientSocketPrt);
      if (monsterData.empty())
        break;

      // Deserialize the monster
      auto monster = MyGame::Sample::GetMonster(monsterData.data());
      verifyMonster(monster);  // asserts if monster is invalid

      // Send confirmation to the client with the monster name
      std::ostringstream oss;
      oss << "Monster " << monster->name()->str() << " verified!";
      asio::write(*clientSocketPrt, asio::buffer(oss.str()));
    }
  } catch (std::exception& e) {
    std::cerr << "Session error: " << e.what() << std::endl;
  }

  std::cout << "Session with client closed" << std::endl;
}

int main() {
  try {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
    std::cout << "Server started on port 12345\n";

    //
    // Accept connections and start new sessions in separate threads
    //
    while (true) {
      auto sock = std::make_shared<tcp::socket>(io_context);
      acceptor.accept(*sock);
      std::thread(session, sock).detach();
    }

    //
  } catch (std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
  }
}
