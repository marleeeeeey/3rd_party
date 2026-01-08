#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <string>
#include <thread>

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#include "../../../DebugLog.h"

using asio::ip::tcp;

class ChatClient {
 public:
  ChatClient(asio::io_context& io_context, asio::ssl::context& ssl_context, const std::string& host, const std::string& port)
      : io_context_(io_context), socket_(io_context, ssl_context) {
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);

    // 1. Connect the underlying TCP socket
    asio::async_connect(socket_.lowest_layer(), endpoints,
                        [this](std::error_code ec, tcp::endpoint) {
                          if (!ec) {
                            debugLog() << "TCP Connected. Starting SSL handshake..." << std::endl;
                            do_handshake();
                          } else {
                            std::cerr << "Connection failed: " << ec.message() << std::endl;
                          }
                        });
  }

  void write(const std::string& msg) {
    // IMPORTANT: This method is called from the main thread
    debugLog() << "ChatClient::write: " << msg << std::endl;

    // Create a copy of the message in the heap with shared ownership.
    auto msgPtr = std::make_shared<std::string>(msg);

    // Post the write to the io_context thread to ensure thread safety
    asio::post(io_context_, [this, msgPtr]  // IMPORTANT: Extend lifetime of msgPtr by capturing it in lambda.
               () {
                 // IMPORTANT: This method is called from the io_context thread
                 debugLog() << "ChatClient::write: asio::post" << std::endl;
                 do_write(msgPtr);  // IMPORTANT: Extend lifetime of msgPtr.
               });
  }

 private:
  void do_handshake() {
    socket_.async_handshake(asio::ssl::stream_base::client,
                            [this](std::error_code ec) {
                              if (!ec) {
                                std::cout << "SSL Handshake successful! Connected to server." << std::endl;
                                do_read();
                              } else {
                                std::cerr << "SSL Handshake failed: " << ec.message() << std::endl;
                              }
                            });
  }

  void do_read() {
    socket_.async_read_some(asio::buffer(read_msg_),
                            [this](std::error_code ec, std::size_t length) {
                              if (!ec) {
                                // Print received message to console
                                std::cout << "\nReceived: " << std::string(read_msg_, length) << "\n> " << std::flush;
                                do_read();  // Wait for more data
                              } else {
                                std::cout << "Disconnected from server." << std::endl;
                                // In SSL, we should shut down the stream
                                std::error_code ignored_ec;
                                socket_.lowest_layer().close(ignored_ec);
                              }
                            });
  }

  void do_write(std::shared_ptr<std::string> msgPtr) {
    debugLog() << "ChatClient::do_write" << std::endl;
    debugLog() << "ChatClient::do_write: msgPtr: " << msgPtr.get() << ", msg:" << *msgPtr << std::endl;
    // IMPORTANT: This method is called from the io_context thread
    asio::async_write(socket_, asio::buffer(*msgPtr),
                      [msgPtr]  // IMPORTANT: Extend the lifetime of msgPtr by capturing it in lambda.
                                // Otherwise, it will be deleted and asio::buffer becomes invalid.
                      (std::error_code ec, std::size_t) {
                        // IMPORTANT: This method is called from the io_context thread
                        debugLog() << "ChatClient::do_write: asio::async_write completed. Callback invoked." << std::endl;
                        if (ec) {
                          std::cerr << "Write failed: " << ec.message() << std::endl;
                        }
                        // IMPORTANT: msgPtr goes out of scope here, and the string is finally deleted
                      });
  }

  asio::io_context& io_context_;
  asio::ssl::stream<tcp::socket> socket_;
  char read_msg_[1024];
};

int main(int argc, char* argv[]) {
  debugLog() << "Starting server (main thread)" << std::endl;
  try {
    asio::io_context io_context;

    // 2. Create SSL context.
    // sslv23 is a generic method that supports various TLS versions.
    asio::ssl::context ssl_context(asio::ssl::context::sslv23);

    // TODO. Optional: Load trusted CAs if you want to verify the server
    ssl_context.set_verify_mode(asio::ssl::verify_peer);
    ssl_context.load_verify_file("server.crt");  // "ca.pem"

    ChatClient client(io_context, ssl_context, "127.0.0.1", "12345");

    // Run Asio loop in a background thread so it doesn't block std::getline
    std::thread t([&io_context]() {
      debugLog() << "Starting io_context loop (background thread)" << std::endl;
      io_context.run();
    });

    std::cout << "Type messages and press Enter to send (or 'exit' to quit):\n> ";
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line == "exit") break;

      // Add a newline for better formatting on other clients
      client.write(line + "\n");
    }

    io_context.stop();
    t.join();

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}