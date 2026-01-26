// Original code: https://github.com/machinezone/IXWebSocket?tab=readme-ov-file#hello-world

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

#include <chrono>
#include <filesystem>
#include <future>
#include <iostream>

#include "../DebugLog.h"

int main() {
  debugLog() << "This logged from main thread" << std::endl;

  // Required on Windows
  ix::initNetSystem();

  // Our websocket object
  ix::WebSocket webSocket;

  // Connect to a server with encryption
  // See https://machinezone.github.io/IXWebSocket/usage/#tls-support-and-configuration
  //     https://github.com/machinezone/IXWebSocket/issues/386#issuecomment-1105235227 (self signed certificates)
  std::string url("wss://echo.websocket.org");

  constexpr bool useLocalhost = false;
  if (useLocalhost) {
    debugLog() << "Using localhost. Build and start local-host server project: uWebSockets_EchoServer_minimalProject." << std::endl;
    url = "wss://localhost:9001";
    ix::SocketTLSOptions tls;
    tls.caFile = "server.crt";
    webSocket.setTLSOptions(tls);
  }

  webSocket.setUrl(url);

  debugLog() << "Connecting to " << url << "..." << std::endl;

  // Create synchronization primitives.
  std::promise<void> openedPromise;
  std::shared_future<void> opened = openedPromise.get_future().share();
  std::promise<std::string> errorPromise;
  std::shared_future<std::string> error = errorPromise.get_future().share();

  // Setup a callback to be fired (in a background thread, watch out for race conditions !)
  // when a message or an event (open, close, error) is received
  webSocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      debugLog() << "received message: " << msg->str << std::endl;
      debugLog() << "> " << std::flush;
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      debugLog() << "Connection established" << std::endl;
      debugLog() << "> " << std::flush;
      try {
        openedPromise.set_value();
      } catch (...) {
      }
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      // Maybe SSL is not configured properly
      debugLog() << "Connection error: " << msg->errorInfo.reason << std::endl;
      debugLog() << "> " << std::flush;
      try {
        errorPromise.set_value(msg->errorInfo.reason);
      } catch (...) {
      }
    }
  });

  // Now that our callback is setup, we can start our background thread and receive messages
  webSocket.start();

  // Wait for a connection to be established
  using namespace std::chrono_literals;
  auto openedStatus = opened.wait_for(5s);
  if (openedStatus != std::future_status::ready) {
    if (error.wait_for(0s) == std::future_status::ready) {
      std::cerr << "Failed to connect: " << error.get() << "\n";
    } else {
      std::cerr << "Timeout waiting for connection\n";
    }
    return 1;
  }

  // Send a message to the server (default to TEXT mode)
  std::cout << std::endl;
  debugLog() << "Sending message 'hello world'..." << std::endl;
  webSocket.send("hello world");

  // Display a prompt
  debugLog() << "> " << std::flush;

  std::string text;
  // Read text from the console and send messages in text mode.
  // Exit with Ctrl-D on Unix or Ctrl-Z on Windows.
  while (std::getline(std::cin, text)) {
    webSocket.send(text);
    debugLog() << "> " << std::flush;
  }

  return 0;
}