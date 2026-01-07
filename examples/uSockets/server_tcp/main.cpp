// Example copied from: https://github.com/uNetworking/uSockets/blob/master/examples/tcp_server.c

#include <libusockets.h>

#include <cassert>
#include <iostream>
#include <string>

#define DISABLE_DEBUG_LOG
#include "../DebugLog.h"
#include "../Globals.h"

long long int clientCount = 0;

/* We don't need any of these */
void on_wakeup(us_loop_t* loop) {
}

void on_pre(us_loop_t* loop) {
}

/* This is not HTTP POST, it is merely an event emitted post-loop iteration */
void on_post(us_loop_t* loop) {
}

us_socket_t* on_tcp_socket_writable(us_socket_t* s) {
  return s;
}

us_socket_t* on_tcp_socket_close(us_socket_t* s, int code, void* reason) {
  clientCount--;
  debugLog() << "Client disconnected. Count=" << clientCount << std::endl;
  return s;
}

us_socket_t* on_tcp_socket_end(us_socket_t* s) {
  /* HTTP does not support half-closed sockets */
  us_socket_shutdown(0, s);
  return us_socket_close(0, s, 0, nullptr);
}

us_socket_t* on_tcp_socket_data(us_socket_t* s, char* data, int length) {
  std::string_view echo(data, length);
  us_socket_write(0, s, echo.data(), echo.size(), 0);
  return s;
}

us_socket_t* on_tcp_socket_open(us_socket_t* s, int is_client, char* ip, int ip_length) {
  clientCount++;
  debugLog() << "Client connected. Total=" << clientCount << std::endl;
  return s;
}

us_socket_t* on_tcp_socket_timeout(us_socket_t* s) {
  return s;
}

int main() {
  /* Create the event loop */
  us_loop_t* loop = us_create_loop(nullptr, on_wakeup, on_pre, on_post, 0);

  assert(loop);

  /* Create a socket context for HTTP */
  us_socket_context_options_t options = {};

  us_socket_context_t* tcp_context = us_create_socket_context(Globals::sslEnabled, loop, 0, options);

  if (!tcp_context) {
    debugLog() << "Could not load Common::SSL cert/key" << std::endl;
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
    debugLog() << "Listening on port " << Globals::port << std::endl;
    us_loop_run(loop);
  } else {
    debugLog() << "Failed to listen!" << std::endl;
  }
}
