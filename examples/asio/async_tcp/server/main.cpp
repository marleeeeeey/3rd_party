#include <asio.hpp>
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
  ClientSession(tcp::socket socket, ChatRoom& room)
      : socket_(std::move(socket)), room_(room) {}

  void start() {
    room_.join(shared_from_this());
    doRead();
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

  tcp::socket socket_;
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
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    doAccept();
  }

 private:
  void doAccept() {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::cout << "Accepted new connection " << socket.remote_endpoint() << std::endl;
            std::make_shared<ClientSession>(std::move(socket), room_)->start();
          }
          doAccept();
        });
  }

  tcp::acceptor acceptor_;
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