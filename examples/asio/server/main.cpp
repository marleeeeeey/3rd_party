#include <asio.hpp>
#include <iostream>
#include <memory>

using asio::ip::tcp;

void session(std::shared_ptr<tcp::socket> clientSocketPrt) {
  try {
    char data[1024];
    for (;;) {
      std::error_code ec;
      size_t length = clientSocketPrt->read_some(asio::buffer(data), ec);
      if (ec == asio::error::eof)
        break;
      else if (ec)
        throw asio::system_error(ec);

      asio::write(*clientSocketPrt, asio::buffer(data, length));
    }
  } catch (std::exception& e) {
    std::cerr << "Session error: " << e.what() << std::endl;
  }
}

int main() {
  try {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

    std::cout << "Server started on port 12345\n";

    for (;;) {
      auto sock = std::make_shared<tcp::socket>(io_context);
      acceptor.accept(*sock);
      std::thread(session, sock).detach();
    }
  } catch (std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
  }
}
