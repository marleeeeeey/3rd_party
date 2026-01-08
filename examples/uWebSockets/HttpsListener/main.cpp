// Example copied from: https://github.com/uNetworking/uWebSockets/blob/master/examples/HelloWorld.cpp

#include "App.h"

/* Note that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support */

int main() {
  /* Overly simple hello world app */
  uWS::SSLApp({.key_file_name = "server.key",
               .cert_file_name = "server.crt",
               .passphrase = "123Qwe!"})
      .get("/*", [](auto* res, auto* /*req*/) {
        res->end("Hello world!");
      })
      .listen(3000, [](auto* listen_socket) {
        if (listen_socket) {
          std::cout << "Listening on port " << 3000 << std::endl;
          std::cout << "Run `curl -k https://localhost:3000` to test" << std::endl;
        }
      })
      .run();

  std::cout << "Failed to listen on port 3000" << std::endl;
}