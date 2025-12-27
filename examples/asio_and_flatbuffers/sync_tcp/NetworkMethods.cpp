#include "NetworkMethods.h"

using asio::ip::tcp;

std::vector<uint8_t> readSizeAndData(std::shared_ptr<tcp::socket> clientSocketPtr) {
  auto& sock = *clientSocketPtr;
  std::error_code ec;

  uint32_t size = 0;
  asio::read(sock, asio::buffer(&size, sizeof(size)), ec);  // 1. read size first

  std::vector<uint8_t> read_buf(size);
  asio::read(sock, asio::buffer(read_buf), ec);  // 2. read payload

  if (ec == asio::error::eof)
    return {};  // Handle error by returning an empty vector
  else if (ec)
    throw asio::system_error(ec);

  return read_buf;  // Data is moved out of the function, it stays alive!
}

void sendSizeAndData(tcp::socket& socket, asio::const_buffer buffer) {
  uint32_t size = static_cast<uint32_t>(buffer.size());
  std::vector<asio::const_buffer> combined_buffers;
  combined_buffers.push_back(asio::buffer(&size, sizeof(size)));  // 1. send size first
  combined_buffers.push_back(buffer);                             // 2. send payload
  asio::write(socket, combined_buffers);
}
