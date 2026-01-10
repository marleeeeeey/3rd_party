// Example from: https://github.com/uNetworking/uWebSockets/blob/master/examples/BroadcastingEchoServer.cpp

#include <atomic>
#include <random>
#include <sstream>

#include "App.h"

std::atomic<int> global_clientCount{0};
struct us_listen_socket_t* global_listen_socket;
constexpr int topicVariations = 4;
constexpr int numberOfTopicsForClient = 2;

int getRandomTopicNumber() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen() % topicVariations;
}

int main() {
  /* ws->getUserData returns one of these */
  struct PerSocketData {
    /* Fill with user data */
    std::vector<std::string> topics;
  };

  /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
   * You may swap to using uWS:App() if you don't need SSL */
  uWS::SSLApp* app = new uWS::SSLApp({/* There are example certificates in uWebSockets.js repo */
                                      .key_file_name = "server.key",
                                      .cert_file_name = "server.crt",
                                      .passphrase = "123Qwe!"});

  app->ws<PerSocketData>("/*", {/* Settings */
                                .compression = uWS::DISABLED,
                                .maxPayloadLength = 16 * 1024 * 1024,
                                .idleTimeout = 60,
                                .maxBackpressure = 16 * 1024 * 1024,
                                .closeOnBackpressureLimit = false,
                                .resetIdleTimeoutOnSend = true,
                                .sendPingsAutomatically = false,
                                /* Handlers */
                                .upgrade = nullptr,
                                .open = [](auto* ws) {
                                  /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */

                                  PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();

                                  auto clientId = (long long)(ws->getUserData());

                                  std::ostringstream topicsOss;
                                  for (int i = 0; i < numberOfTopicsForClient; i++) {
                                    std::string topic = std::to_string(getRandomTopicNumber());
                                    perSocketData->topics.push_back(topic);
                                    ws->subscribe(topic);
                                    topicsOss << topic << " ";
                                  }
                                  ++global_clientCount;
                                  std::cout << "client (" << clientId << ") connected and subscribed to topics:" << topicsOss.str() << ". total=" << global_clientCount.load() << std::endl;
                                  ws->send("You (" + std::to_string(clientId) + ") connected and subscribed to topics: " + topicsOss.str(), uWS::OpCode::TEXT); },
                                .message = [&app](auto* ws, std::string_view message, uWS::OpCode opCode) {
                                  std::cout << "recv: " << message << std::endl;

                                  PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();

                                  auto clientId = (long long)(ws->getUserData());

                                  for (auto topic : perSocketData->topics) {
                                    std::ostringstream oss;
                                    oss << clientId << ": app->publish to topic=" << topic << " message=" << message;
                                    app->publish(topic, oss.str(), opCode); // CAN BE CALLED OUTSIDE OF WS HANDLER
                                                                            // INCLUDE SENDING TO HIMSELF
                                  }

                                  for (auto topic : perSocketData->topics) {
                                    std::ostringstream oss;
                                    oss << clientId << ":  ws->publish to topic=" << topic << " message=" << message;
                                    ws->publish(topic, oss.str(), opCode);  // CAN ONLY BE CALLED FROM WITHIN WS HANDLER
                                                                            // EXCLUDE SENDING TO HIMSELF
                                  } },
                                .drain = [](auto* /*ws*/) {
                                  /* Check ws->getBufferedAmount() here */
                                  // std::cout << "drain" << std::endl;
                                },
                                .ping = [](auto* /*ws*/, std::string_view) {
                                  /* Not implemented yet */ },
                                .pong = [](auto* /*ws*/, std::string_view) {
                                  /* Not implemented yet */ },
                                .close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
                                  /* You may access ws->getUserData() here */
                                  --global_clientCount;
                                  auto clientId = (long long)(ws->getUserData());
                                  std::cout << "client (" << clientId << ") disconnected, total=" << global_clientCount.load() << std::endl; }})
      .listen(9001, [](auto* listen_s) {
        if (listen_s) {
          std::cout << "Listening on port " << 9001 << std::endl;
          // listen_socket = listen_s;
        }
      });

  app->run();

  delete app;

  uWS::Loop::get()->free();
}
