// Example copied from: https://github.com/uNetworking/uSockets/blob/master/examples/tcp_load_test.c

#include <libusockets.h>

#include <iostream>

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#define DEBUG_LOG_DISABLE_VERBOSE_LEVEL
#define DEBUG_LOG_USER_PREFIX "[CLIENT]"
#include "../../../DebugLog.h"
#include "../Globals.h"

std::string host = "127.0.0.1";
std::string clientRequestMsg = "Message for ping-pong";
int numberOfConnections = 20;
int responses;

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
  return s;
}

us_socket_t* on_tcp_socket_end(us_socket_t* s) {
  debugLog() << "on_tcp_socket_end" << std::endl;
  return us_socket_close(Globals::sslEnabled, s, 0, nullptr);
}

us_socket_t* on_tcp_socket_data(us_socket_t* s, char* data, int length) {
  debugLog() << "on_tcp_socket_data" << std::endl;
  us_socket_write(Globals::sslEnabled, s, clientRequestMsg.c_str(), clientRequestMsg.size(), 0);
  responses++;
  return s;
}

us_socket_t* on_tcp_socket_open(us_socket_t* s, int is_client, char* ip, int ip_length) {
  debugLog() << "on_tcp_socket_open" << std::endl;
  /* Send a request */
  us_socket_write(Globals::sslEnabled, s, clientRequestMsg.c_str(), clientRequestMsg.size(), 0);

  if (--numberOfConnections) {
    // Initiate another connection
    us_socket_context_connect(Globals::sslEnabled, us_socket_context(Globals::sslEnabled, s), host.c_str(), Globals::port, nullptr, 0, 0);
  } else {
    debugLog() << "All connections established" << std::endl;
    debugLog() << "Running benchmark now..." << std::endl;

    constexpr int seconds = LIBUS_TIMEOUT_GRANULARITY;  // 4 seconds
    constexpr int minutes = 1;
    us_socket_timeout(Globals::sslEnabled, s, seconds);       // Start high-resolution frequency timeout (seconds)
    us_socket_long_timeout(Globals::sslEnabled, s, minutes);  // Start low-resolution frequency timeout (minutes)
  }

  return s;
}

us_socket_t* on_tcp_socket_long_timeout(us_socket_t* s) {
  debugLog() << "on_tcp_socket_long_timeout" << std::endl;
  /* Print current statistics */
  std::cout << "--- Minute mark ---" << std::endl;

  // Start timeout:
  constexpr int minutes = 1;
  us_socket_long_timeout(Globals::sslEnabled, s, minutes);  // Start low-resolution frequency timeout (minutes)

  return s;
}

us_socket_t* on_tcp_socket_timeout(us_socket_t* s) {
  debugLog() << "on_tcp_socket_timeout" << std::endl;
  /* Print current statistics */
  constexpr int seconds = LIBUS_TIMEOUT_GRANULARITY;  // 4 seconds

  std::cout << "Req/sec: " << (float)responses / seconds << std::endl;  // Show performance metrics

  responses = 0;  // Reset statistics

  us_socket_timeout(Globals::sslEnabled, s, seconds);  // Start high-resolution frequency timeout (seconds)

  return s;
}

us_socket_t* on_tcp_socket_connect_error(us_socket_t* s, int code) {
  debugLog() << "on_tcp_socket_connect_error" << std::endl;
  debugLog() << "Cannot connect to server" << std::endl;
  return s;
}

int main() {
  // clang-format off
  std::cout << "This is client-server benchmark for uSockets (TCP).\n"
               "1. Client creates XXX TCP connections to server.\n"
               "2. Client and server ping pong messages and calculate "
               "average throughput in requests per second every 4 seconds. \n"
               "\n"
               "* Long timeout equals 1 minute is used for informing only.\n"
               "** add `#define DISABLE_DEBUG_LOG` to disable debug logging to measure real throughput."
               "You may use debugger to see how many requests are sent per second.\n"
               "---\n"
               "numberOfConnections=" << numberOfConnections << "\n"
               "Globals::sslEnabled=" << Globals::sslEnabled << "\n"
               "---\n"
            << std::endl;
  // clang-format on

  /* Create the event loop */
  us_loop_t* loop = us_create_loop(nullptr, on_wakeup, on_pre, on_post, 0);

  /* Create a socket context for HTTP */
  us_socket_context_options_t options = {
      .ca_file_name = "server.crt"};

  us_socket_context_t* tcp_context = us_create_socket_context(Globals::sslEnabled, loop, 0, options);

  if (!tcp_context) {
    std::cerr << "Could not load SSL cert/key" << std::endl;
    exit(0);
  }

  /* Set up event handlers */
  us_socket_context_on_open(Globals::sslEnabled, tcp_context, on_tcp_socket_open);
  us_socket_context_on_data(Globals::sslEnabled, tcp_context, on_tcp_socket_data);
  us_socket_context_on_writable(Globals::sslEnabled, tcp_context, on_tcp_socket_writable);
  us_socket_context_on_close(Globals::sslEnabled, tcp_context, on_tcp_socket_close);
  us_socket_context_on_timeout(Globals::sslEnabled, tcp_context, on_tcp_socket_timeout);
  us_socket_context_on_long_timeout(Globals::sslEnabled, tcp_context, on_tcp_socket_long_timeout);
  us_socket_context_on_end(Globals::sslEnabled, tcp_context, on_tcp_socket_end);
  us_socket_context_on_connect_error(Globals::sslEnabled, tcp_context, on_tcp_socket_connect_error);

  /* Start making HTTP connections */
  if (!us_socket_context_connect(Globals::sslEnabled, tcp_context, host.c_str(), Globals::port, nullptr, 0, 0)) {
    std::cerr << "Cannot connect to server" << std::endl;
  }

  us_loop_run(loop);

  us_socket_context_free(Globals::sslEnabled, tcp_context);
  us_loop_free(loop);
}
