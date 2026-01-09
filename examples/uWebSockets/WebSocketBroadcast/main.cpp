// Example from: https://github.com/uNetworking/uWebSockets/blob/master/examples/Broadcast.cpp

/* We simply call the root header file "App.h", giving you uWS::App and uWS::SSLApp */
#include <time.h>

#include <iostream>

#include "App.h"

/* This is a simple WebSocket echo server example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */

uWS::SSLApp* globalApp;
int globalSocketCount = 0;

int main() {
  std::cout << "Broadcast WebSocket server\n"
               "How to use:\n"
               "  1. Start server (this file)\n"
               "  2. Open https://localhost:9001 to accept self-signed certificate\n"
               "  3. Start client.html in browser\n"
            << std::endl;

  /* ws->getUserData returns one of these */
  struct PerSocketData {
    /* Fill with user data */
  };

  /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
   * You may swap to using uWS:App() if you don't need SSL */
  uWS::SSLApp app = uWS::SSLApp(
                        {// Options
                         .key_file_name = "server.key",
                         .cert_file_name = "server.crt",
                         .passphrase = "123Qwe!"}
                        //
                        )
                        .ws<PerSocketData>("/*", {/* Settings */
                                                  .compression = uWS::SHARED_COMPRESSOR,
                                                  .maxPayloadLength = 16 * 1024 * 1024,
                                                  .idleTimeout = 16,
                                                  .maxBackpressure = 1 * 1024 * 1024,
                                                  .closeOnBackpressureLimit = false,
                                                  .resetIdleTimeoutOnSend = false,
                                                  .sendPingsAutomatically = true,
                                                  /* Handlers */
                                                  .upgrade = nullptr,
                                                  .open = [](auto* ws) {
                                                    /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
                                                    globalSocketCount++;
                                                    std::cout << "client connected. globalSocketCount=" << globalSocketCount << std::endl;
                                                    ws->subscribe("broadcast"); },
                                                  .message = [](auto* /*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {

                                                  },
                                                  .drain = [](auto* /*ws*/) {
                                                    /* Check ws->getBufferedAmount() here */ },
                                                  .ping = [](auto* /*ws*/, std::string_view) {
                                                    /* Not implemented yet */ },
                                                  .pong = [](auto* /*ws*/, std::string_view) {
                                                    /* Not implemented yet */ },
                                                  .close = [](auto* /*ws*/, int /*code*/, std::string_view /*message*/) {
                                                    /* You may access ws->getUserData() here */
                                                    globalSocketCount--;
                                                    std::cout << "client disconnected. globalSocketCount=" << globalSocketCount << std::endl; }})
                        .listen(9001, [](auto* listen_socket) {
                          if (listen_socket) {
                            std::cout << "Listening on port " << 9001 << std::endl;
                          }
                        });

  globalApp = &app;

  struct us_loop_t* loop = (struct us_loop_t*)uWS::Loop::get();
  struct us_timer_t* delayTimer = us_create_timer(loop, 0, 0);

  auto delay = 300;  // millis

  // broadcast the unix time as millis every `delay` millis
  us_timer_set(delayTimer, [](struct us_timer_t* /*t*/) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);


    int64_t millis = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    // std::cout << "Broadcasting timestamp: " << millis << std::endl;

    globalApp->publish("broadcast", std::string_view((char*)&millis, sizeof(millis)), uWS::OpCode::BINARY, false); }, delay, delay);

  app.run();
}
