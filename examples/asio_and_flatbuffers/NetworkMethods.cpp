#include "NetworkMethods.h"

#include "Serialization.h"

using asio::ip::tcp;

std::vector<uint8_t> readMonsterData(std::shared_ptr<tcp::socket> clientSocketPtr) {
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

void sendMonsterData(tcp::socket& socket, const flatbuffers::FlatBufferBuilder& builder) {
  // TODO: maybe better to recieve arguments: data pointer and size - to remove dependency on FlatBufferBuilder?
  uint8_t* buf = builder.GetBufferPointer();
  uint32_t size = static_cast<uint32_t>(builder.GetSize());
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(asio::buffer(&size, sizeof(size)));  // 1. send size first
  buffers.push_back(asio::buffer(buf, size));            // 2. send payload
  asio::write(socket, buffers);
}
