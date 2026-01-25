// Original code: https://github.com/machinezone/IXWebSocket?tab=readme-ov-file#hello-world

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

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
  webSocket.setUrl(url);

  debugLog() << "Connecting to " << url << "..." << std::endl;

  // Setup a callback to be fired (in a background thread, watch out for race conditions !)
  // when a message or an event (open, close, error) is received
  webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      debugLog() << "received message: " << msg->str << std::endl;
      debugLog() << "> " << std::flush;
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      debugLog() << "Connection established" << std::endl;
      debugLog() << "> " << std::flush;
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      // Maybe SSL is not configured properly
      debugLog() << "Connection error: " << msg->errorInfo.reason << std::endl;
      debugLog() << "> " << std::flush;
    }
  });

  // Now that our callback is setup, we can start our background thread and receive messages
  webSocket.start();

  // Send a message to the server (default to TEXT mode)
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