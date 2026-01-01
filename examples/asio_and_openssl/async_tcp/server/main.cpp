#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#define DISABLE_DEBUG_LOG
#include "../DebugLog.h"

using asio::ip::tcp;

// Forward declaration
class ClientSession;

// Represents a chat room for managing client sessions and broadcasting messages.
// This application has only one room.
class ChatRoom {
 public:
  void join(std::shared_ptr<ClientSession> session) {
    sessions_.insert(session);
    std::cout << "Client joined. Total clients: " << sessions_.size() << std::endl;
  }

  void leave(std::shared_ptr<ClientSession> session) {
    sessions_.erase(session);
    std::cout << "Client left. Total clients: " << sessions_.size() << std::endl;
  }

  void deliver(const std::string& msg) {
    for (auto& session : sessions_) {
      deliverMessage(session, msg);
    }
  }

 private:
  // Using a helper to trigger the write on the session
  void deliverMessage(std::shared_ptr<ClientSession> session, const std::string& msg);

  std::set<std::shared_ptr<ClientSession>> sessions_;
};

// Represents a single client connection
class ClientSession : public std::enable_shared_from_this<ClientSession> {
 public:
  ClientSession(tcp::socket socket, asio::ssl::context& context, ChatRoom& room)
      : socket_(std::move(socket), context), room_(room) {}

  void start() {
    auto self(shared_from_this());
    socket_.async_handshake(asio::ssl::stream_base::server,
                            [this, self](std::error_code ec) {
                              if (!ec) {
                                room_.join(shared_from_this());
                                doRead();
                              }
                            });
  }

  void deliver(const std::string& msg) {
    auto self(shared_from_this());
    auto msgCopy = std::make_shared<std::string>(msg);

    asio::async_write(socket_, asio::buffer(*msgCopy),
                      [self, msgCopy]  // Extent lifetime for self and asio::buffer
                      (std::error_code ec, std::size_t /*length*/) {
                        // If error occurs, the session will eventually be dropped by the read loop
                      });
  }

 private:
  void doRead() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, max_length),
                            [this, self]  // Extent lifetime for self
                            (std::error_code ec, std::size_t length) {
                              if (!ec) {
                                std::string msg(data_, length);
                                std::cout << "Broadcasting: " << msg;
                                room_.deliver(msg);  // Send to everyone
                                doRead();            // Wait for next message
                              } else {
                                room_.leave(shared_from_this());
                              }
                            });
  }

  asio::ssl::stream<tcp::socket> socket_;
  ChatRoom& room_;

  enum { max_length = 1024 };

  char data_[max_length];
};

void ChatRoom::deliverMessage(std::shared_ptr<ClientSession> session, const std::string& msg) {
  session->deliver(msg);
}

// Creates one room to place all new clients (connections) to this room.
class ChatServer {
 public:
  ChatServer(asio::io_context& io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        ssl_context_(asio::ssl::context::sslv23) {
    // Configure SSL context (In real app, load your certificates here)
    ssl_context_.set_options(asio::ssl::context::default_workarounds |
                             asio::ssl::context::no_sslv2 |
                             asio::ssl::context::single_dh_use);

    // TODO For testing purposes only:
    ssl_context_.use_certificate_chain_file("server.crt");
    ssl_context_.use_private_key_file("server.key", asio::ssl::context::pem);

    doAccept();
  }

 private:
  void doAccept() {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<ClientSession>(std::move(socket), ssl_context_, room_)->start();
          }
          doAccept();
        });
  }

  tcp::acceptor acceptor_;
  asio::ssl::context ssl_context_;
  ChatRoom room_;
};

int main() {
  debugLog() << "Starting server (MAIN THREAD)" << std::endl;

  try {
    asio::io_context io_context;
    ChatServer s(io_context, 12345);
    std::cout << "Async Chat Server started on port 12345..." << std::endl;
    debugLog() << " ASIO IO context running in MAIN THREAD also" << std::endl;
    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}