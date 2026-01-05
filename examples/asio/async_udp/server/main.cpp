#include <asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <map>

#include "../../DebugLog.h"

using asio::ip::udp;

// Represents a chat room for managing client sessions and broadcasting messages.
// This application has only one room.
class ChatRoom {
public:
    void add_participant(const udp::endpoint& endpoint) {
        participants_.insert(endpoint);
        if (participants_.size() > last_size_) {
            std::cout << "New participant: " << endpoint << ". Total: " << participants_.size() << std::endl;
            last_size_ = participants_.size();
        }
    }

    void deliver(const std::string& msg, udp::socket& socket) {
        auto msg_ptr = std::make_shared<std::string>(msg);
        for (const auto& participant : participants_) {
            socket.async_send_to(asio::buffer(*msg_ptr), participant,
                [msg_ptr](std::error_code /*ec*/, std::size_t /*bytes*/) {
                    debugLog() << "Sent message to participant: size=" << msg_ptr->size() << ". Package may be lost." << std::endl;
                });
        }
    }

private:
    std::set<udp::endpoint> participants_;
    size_t last_size_ = 0;
};

class AsyncUdpServer {
public:
    AsyncUdpServer(asio::io_context& io_context, short port)
        : socket_(io_context, udp::endpoint(udp::v4(), port)) {
        doReceive();
    }

private:
    void doReceive() {
        // Wait data from any source
        socket_.async_receive_from(
            asio::buffer(data_, max_length), remote_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    // 1. Add a new participant to the room
                    room_.add_participant(remote_endpoint_);

                    // 2. Broadcast received a message to all participants
                    std::string msg(data_, bytes_recvd);
                    std::cout << "Received " << bytes_recvd << " bytes from " << remote_endpoint_ << std::endl;

                    room_.deliver(msg, socket_);

                    // 3. Wait for the next message
                    doReceive();
                } else {
                    std::cerr << "Receive error: " << ec.message() << std::endl;
                    doReceive();
                }
            });
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    ChatRoom room_;
    enum { max_length = 1024 };
    char data_[max_length];
};

int main() {
    try {
        asio::io_context io_context;
        AsyncUdpServer s(io_context, 12345);
        std::cout << "Async UDP Chat Server started on port 12345..." << std::endl;
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}