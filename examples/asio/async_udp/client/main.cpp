#include <asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "../../../DebugLog.h"

using asio::ip::udp;

class AsyncUdpClient {
public:
    AsyncUdpClient(asio::io_context& io_context, const std::string& host, short port)
        : io_context_(io_context),
          socket_(io_context, udp::endpoint(udp::v4(), 0)) { // 0 - OS will pick any free port

        udp::resolver resolver(io_context);
        server_endpoint_ = *resolver.resolve(udp::v4(), host, std::to_string(port)).begin();

        std::cout << "UDP Client initialized. Server: " << server_endpoint_ << std::endl;

        // Wait for server responses
        do_receive();
    }

    void write(const std::string& msg) {
        auto msg_ptr = std::make_shared<std::string>(msg);

        // Post the write to the io_context thread to ensure thread safety
        asio::post(io_context_, [this, msg_ptr]() {
            do_send(msg_ptr);
        });
    }

private:
    void do_send(std::shared_ptr<std::string> msg_ptr) {
        socket_.async_send_to(
            asio::buffer(*msg_ptr), server_endpoint_,
            [msg_ptr](std::error_code ec, std::size_t /*bytes_sent*/) {
                if (ec) {
                    std::cerr << "Send error: " << ec.message() << std::endl;
                }
            });
    }

    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(recv_buf_, max_length), sender_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    std::cout << "\rReceived: " << std::string(recv_buf_, bytes_recvd) << "\n> " << std::flush;
                    do_receive(); // Wait for next message
                } else if (ec != asio::error::operation_aborted) {
                    std::cerr << "Receive error: " << ec.message() << std::endl;
                    do_receive(); // Wait for next message
                }
            });
    }

    asio::io_context& io_context_;
    udp::socket socket_;
    udp::endpoint server_endpoint_;
    udp::endpoint sender_endpoint_; // Sender endpoint. More probably this is a server endpoint.

    enum { max_length = 1024 };
    char recv_buf_[max_length];
};

int main() {
    try {
        asio::io_context io_context;

        AsyncUdpClient client(io_context, "127.0.0.1", 12345);

        // Run Asio loop in a background thread so it doesn't block std::getline
        std::thread t([&io_context]() { io_context.run(); });

        std::cout << "Type messages and press Enter (or 'exit' to quit):\n> ";
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "exit") break;
            client.write(line);
            std::cout << "> ";
        }

        io_context.stop();
        if (t.joinable()) t.join();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}