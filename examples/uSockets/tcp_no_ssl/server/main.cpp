// Example copied from: https://github.com/uNetworking/uSockets/blob/master/examples/tcp_server.c

#include <libusockets.h>

#include <cassert>
#include <iostream>
#include <string>

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#define DEBUG_LOG_DISABLE_VERBOSE_LEVEL
#define DEBUG_LOG_USER_PREFIX "[SERVER]"
#include "../../../DebugLog.h"
#include "../Globals.h"

long long int clientCount = 0;

/* We don't need any of these */
void on_wakeup(us_loop_t* loop) {
  verboseLog() << "on_wakeup" << std::endl;
}

void on_pre(us_loop_t* loop) {
  verboseLog() << "on_pre" << std::endl;
}

/* This is not HTTP POST, it is merely an event emitted post-loop iteration */
void on_post(us_loop_t* loop) {
  verboseLog() << "on_post" << std::endl;
}

us_socket_t* on_tcp_socket_writable(us_socket_t* s) {
  debugLog() << "on_tcp_socket_writable" << std::endl;
  return s;
}

us_socket_t* on_tcp_socket_close(us_socket_t* s, int code, void* reason) {
  debugLog() << "on_tcp_socket_close" << std::endl;
  clientCount--;
  std::cout << "Client disconnected. Count=" << clientCount << std::endl;
  return s;
}

us_socket_t* on_tcp_socket_end(us_socket_t* s) {
  debugLog() << "on_tcp_socket_end" << std::endl;
  /* HTTP does not support half-closed sockets */
  us_socket_shutdown(Globals::sslEnabled, s);
  return us_socket_close(Globals::sslEnabled, s, 0, nullptr);
}

us_socket_t* on_tcp_socket_data(us_socket_t* s, char* data, int length) {
  debugLog() << "on_tcp_socket_data" << std::endl;
  std::string_view echo(data, length);
  debugLog() << "Received: " << echo << std::endl;
  us_socket_write(Globals::sslEnabled, s, echo.data(), echo.size(), 0);
  return s;
}

us_socket_t* on_tcp_socket_open(us_socket_t* s, int is_client, char* ip, int ip_length) {
  debugLog() << "on_tcp_socket_open" << std::endl;
  clientCount++;
  std::cout << "Client connected. Total=" << clientCount << std::endl;
  return s;
}

us_socket_t* on_tcp_socket_timeout(us_socket_t* s) {
  debugLog() << "on_tcp_socket_timeout" << std::endl;
  return s;
}

int main() {
  std::cout << "Globals::sslEnabled=" << Globals::sslEnabled << std::endl;
  /* Create the event loop */
  us_loop_t* loop = us_create_loop(nullptr, on_wakeup, on_pre, on_post, 0);
  assert(loop);

  /* Create a socket context for HTTP */
  us_socket_context_options_t options = {
      .key_file_name = "server.key",
      .cert_file_name = "server.crt",
      .passphrase = "123Qwe!",
  };

  us_socket_context_t* tcp_context = us_create_socket_context(Globals::sslEnabled, loop, 0, options);

  if (!tcp_context) {
    std::cerr << "Could not load Common::SSL cert/key" << std::endl;
    exit(0);
  }

  /* Set up event handlers */
  us_socket_context_on_open(Globals::sslEnabled, tcp_context, on_tcp_socket_open);
  us_socket_context_on_data(Globals::sslEnabled, tcp_context, on_tcp_socket_data);
  us_socket_context_on_writable(Globals::sslEnabled, tcp_context, on_tcp_socket_writable);
  us_socket_context_on_close(Globals::sslEnabled, tcp_context, on_tcp_socket_close);
  us_socket_context_on_timeout(Globals::sslEnabled, tcp_context, on_tcp_socket_timeout);
  us_socket_context_on_end(Globals::sslEnabled, tcp_context, on_tcp_socket_end);

  /* Start serving HTTP connections */
  us_listen_socket_t* listen_socket = us_socket_context_listen(Globals::sslEnabled, tcp_context, nullptr, Globals::port, 0, 0);

  if (listen_socket) {
    std::cout << "Listening on port " << Globals::port << std::endl;
    us_loop_run(loop);
  } else {
    std::cerr << "Failed to listen!" << std::endl;
  }
}
