// Example copied from: https://github.com/uNetworking/uSockets/blob/master/examples/tcp_server.c

#include <libusockets.h>
#include <iostream>
#include <string>

constexpr int SSL = 0;
long long int clientCount = 0;

/* We don't need any of these */
void on_wakeup(us_loop_t *loop) {
}

void on_pre(us_loop_t *loop) {
}

/* This is not HTTP POST, it is merely an event emitted post-loop iteration */
void on_post(us_loop_t *loop) {
}

us_socket_t *on_tcp_socket_writable(us_socket_t *s) {
  return s;
}

us_socket_t *on_tcp_socket_close(us_socket_t *s, int code, void *reason) {
  clientCount--;
  std::cout << "Client disconnected. Count=" << clientCount << std::endl;
  return s;
}

us_socket_t *on_tcp_socket_end(us_socket_t *s) {
  /* HTTP does not support half-closed sockets */
  us_socket_shutdown(0, s);
  return us_socket_close(0, s, 0, nullptr);
}

us_socket_t *on_tcp_socket_data(us_socket_t *s, char *data, int length) {
  us_socket_write(0, s, "Hello short message!", 20, 0);
  return s;
}

us_socket_t *on_tcp_socket_open(us_socket_t *s, int is_client, char *ip, int ip_length) {
  clientCount++;
  std::cout << "Client connected. Total=" << clientCount << std::endl;
  return s;
}

us_socket_t *on_tcp_socket_timeout(us_socket_t *s) {
  return s;
}

int main() {
  /* Create the event loop */
  us_loop_t *loop = us_create_loop(nullptr, on_wakeup, on_pre, on_post, 0);

  /* Create a socket context for HTTP */
  us_socket_context_options_t options = {};

  us_socket_context_t *tcp_context = us_create_socket_context(0, loop, 0, options);

  if (!tcp_context) {
    std::cout << "Could not load SSL cert/key" << std::endl;
    exit(0);
  }

  /* Set up event handlers */
  us_socket_context_on_open(SSL, tcp_context, on_tcp_socket_open);
  us_socket_context_on_data(SSL, tcp_context, on_tcp_socket_data);
  us_socket_context_on_writable(SSL, tcp_context, on_tcp_socket_writable);
  us_socket_context_on_close(SSL, tcp_context, on_tcp_socket_close);
  us_socket_context_on_timeout(SSL, tcp_context, on_tcp_socket_timeout);
  us_socket_context_on_end(SSL, tcp_context, on_tcp_socket_end);

  /* Start serving HTTP connections */
  us_listen_socket_t *listen_socket = us_socket_context_listen(SSL, tcp_context, nullptr, 12345, 0, 0);

  if (listen_socket) {
    std::cout << "Listening on port 12345..." << std::endl;
    us_loop_run(loop);
  } else {
    std::cout << "Failed to listen!" << std::endl;
  }
}
