#pragma once

namespace Globals {
constexpr int sslEnabled = 0;  // This example doesn't work with SSL, so we use '0'
constexpr int port = 12345;
}  // namespace Globals

/*
Globals::sslEnabled = 0
[CLIENT][DEBUG][74088] on_tcp_socket_open
[CLIENT][DEBUG][74088] All connections established
[CLIENT][DEBUG][74088] Running benchmark now...
[CLIENT][DEBUG][74088] on_tcp_socket_data - A LOT OF THESE MESSAGES
...
[CLIENT][DEBUG][74088] on_tcp_socket_timeout
[CLIENT][DEBUG][74088] Req/sec: 7756

[SERVER][DEBUG][100632] Listening on port 12345
[SERVER][DEBUG][100632] on_tcp_socket_open
[SERVER][DEBUG][100632] Client connected. Total=1
[SERVER][DEBUG][100632] on_tcp_socket_data
[SERVER][DEBUG][100632] Received: Message for ping-pong

---

Globals::sslEnabled = 1 - PROBLEM HERE ???
[CLIENT][DEBUG][90688] on_tcp_socket_open
[CLIENT][DEBUG][90688] All connections established
[CLIENT][DEBUG][90688] Running benchmark now...
[CLIENT][DEBUG][90688] on_tcp_socket_writable
[CLIENT][DEBUG][90688] on_tcp_socket_timeout
[CLIENT][DEBUG][90688] Req/sec: 0

[SERVER][DEBUG][55796] Listening on port 12345
[SERVER][DEBUG][55796] on_tcp_socket_open
[SERVER][DEBUG][55796] Client connected. Total=1
*/