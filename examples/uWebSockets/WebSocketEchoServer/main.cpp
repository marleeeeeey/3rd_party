// Example from https://github.com/uNetworking/uWebSockets/blob/master/examples/EchoServer.cpp

/* We simply call the root header file "App.h", giving you uWS::App and uWS::SSLApp */
#include "../../DebugLog.h"
#include "App.h"

/* This is a simple WebSocket echo server example.
 * You may compile it with "WITH_OPENSSL=1 make" or with "make" */

int main() {
  /* ws->getUserData returns one of these */
  struct PerSocketData {
    /* Fill with user data */
  };

  /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
   * You may swap to using uWS:App() if you don't need SSL */
  uWS::SSLApp(
      {/* There are example certificates in uWebSockets.js repo */
       .key_file_name = "server.key",
       .cert_file_name = "server.crt",
       .passphrase = "123Qwe!"})
      .ws<PerSocketData>("/*",
                         {/* Settings */
                          .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR | uWS::DEDICATED_DECOMPRESSOR),
                          .maxPayloadLength = 100 * 1024 * 1024,
                          .idleTimeout = 16,
                          .maxBackpressure = 100 * 1024 * 1024,
                          .closeOnBackpressureLimit = false,
                          .resetIdleTimeoutOnSend = false,
                          .sendPingsAutomatically = true,
                          /* Handlers */
                          .upgrade = nullptr,
                          .open = [](auto* ws) {
                            /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
                            debugLog() << "ws.open" << std::endl; },
                          .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                            /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
                             * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
                             * benchmarking of large message sending without compression */
                             /* Never mind, it changed back to never compressing for now */
                            debugLog() << "ws.message: " << message << std::endl;
                            ws->send(message, opCode, false); },
                          .dropped = [](auto* /*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
                            /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
                            debugLog() << "ws.dropped" << std::endl; },
                          .drain = [](auto* /*ws*/) {
                            /* Check ws->getBufferedAmount() here */
                            debugLog() << "ws.drain" << std::endl; },
                          .ping = [](auto* /*ws*/, std::string_view) {
                            /* Not implemented yet */
                            debugLog() << "ws.ping" << std::endl; },
                          .pong = [](auto* /*ws*/, std::string_view) {
                            /* Not implemented yet */
                            debugLog() << "ws.pong" << std::endl; },
                          .close = [](auto* /*ws*/, int /*code*/, std::string_view /*message*/) {
                            /* You may access ws->getUserData() here */
                            debugLog() << "ws.close" << std::endl; }})
      .listen(9001, [](auto* listen_socket) {
        if (listen_socket) {
          std::cout << "Listening on port " << 9001 << std::endl;
          std::cout << "  1. Open https://localhost:9001 to accept self-signed certificate" << std::endl;
          std::cout << "  2. Open https://piehost.com/websocket-tester" << std::endl;
          std::cout << "  3. Run wss://localhost:9001" << std::endl;
        }
      })
      .run();
}