#pragma once

#include <asio.hpp>

// Read size of data first and then read data with that size.
// USAGE EXAMPLE:
// std::vector<uint8_t> flatdata = readSizeAndData(clientSocketPrt);
// if (flatdata.empty()) break;
// auto monster = MyGame::Sample::GetMonster(flatdata.data()); // Deserialize
std::vector<uint8_t> readSizeAndData(std::shared_ptr<asio::ip::tcp::socket> clientSocketPtr);

// Send size of data first and then send data with that size.
// USAGE EXAMPLE:
// flatbuffers::FlatBufferBuilder builder = createMonster(line);
// sendSizeAndData(socket, asio::buffer(builder.GetBufferPointer(), builder.GetSize()));
void sendSizeAndData(asio::ip::tcp::socket& socket, asio::const_buffer buffer);
