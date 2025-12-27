#pragma once

#include <flatbuffers/flatbuffer_builder.h>

#include <asio.hpp>

// Read monster from socket. Return empty vector on error
std::vector<uint8_t> readMonsterData(std::shared_ptr<asio::ip::tcp::socket> clientSocketPtr);

// Send monster data to socket
void sendMonsterData(asio::ip::tcp::socket& socket, const flatbuffers::FlatBufferBuilder& builder);