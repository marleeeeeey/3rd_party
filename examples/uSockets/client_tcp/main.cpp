// Example copied from: https://github.com/uNetworking/uSockets/blob/master/examples/tcp_load_test.c

#include <libusockets.h>
constexpr int SSL = 0;

#include <iostream>

std::string request = "Hello there!";
std::string host;
int port;
int connections;

int responses;

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
  return s;
}

us_socket_t* on_tcp_socket_end(us_socket_t* s) {
  return us_socket_close(SSL, s, 0, nullptr);
}

us_socket_t* on_tcp_socket_data(us_socket_t* s, char* data, int length) {
  us_socket_write(SSL, s, request.c_str(), request.size() - 1, 0);

  responses++;

  return s;
}

us_socket_t* on_tcp_socket_open(us_socket_t* s, int is_client, char* ip, int ip_length) {
  /* Send a request */
  us_socket_write(SSL, s, request.c_str(), request.size() - 1, 0);

  if (--connections) {
    us_socket_context_connect(SSL, us_socket_context(SSL, s), host.c_str(), port, nullptr, 0, 0);
  } else {
    std::cout << "Running benchmark now..." << std::endl;

    us_socket_timeout(SSL, s, LIBUS_TIMEOUT_GRANULARITY);
    us_socket_long_timeout(SSL, s, 1);
  }

  return s;
}

us_socket_t* on_tcp_socket_long_timeout(us_socket_t* s) {
  /* Print current statistics */
  std::cout << "--- Minute mark ---" << std::endl;
  us_socket_long_timeout(SSL, s, 1);

  return s;
}

us_socket_t* on_tcp_socket_timeout(us_socket_t* s) {
  /* Print current statistics */
  std::cout << "Req/sec: " << (float)responses / LIBUS_TIMEOUT_GRANULARITY << std::endl;

  responses = 0;
  us_socket_timeout(SSL, s, LIBUS_TIMEOUT_GRANULARITY);

  return s;
}

us_socket_t* on_tcp_socket_connect_error(us_socket_t* s, int code) {
  std::cout << "Cannot connect to server" << std::endl;

  return s;
}

int main() {
  port = 12345;
  host = "127.0.0.1";
  connections = 50;

  /* Create the event loop */
  us_loop_t* loop = us_create_loop(nullptr, on_wakeup, on_pre, on_post, 0);

  /* Create a socket context for HTTP */
  us_socket_context_options_t options = {};
  us_socket_context_t* tcp_context = us_create_socket_context(SSL, loop, 0, options);

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
  us_socket_context_on_long_timeout(SSL, tcp_context, on_tcp_socket_long_timeout);
  us_socket_context_on_end(SSL, tcp_context, on_tcp_socket_end);
  us_socket_context_on_connect_error(SSL, tcp_context, on_tcp_socket_connect_error);

  /* Start making HTTP connections */
  if (!us_socket_context_connect(SSL, tcp_context, host.c_str(), port, nullptr, 0, 0)) {
    std::cout << "Cannot connect to server" << std::endl;
  }

  us_loop_run(loop);

  us_socket_context_free(SSL, tcp_context);
  us_loop_free(loop);
}
