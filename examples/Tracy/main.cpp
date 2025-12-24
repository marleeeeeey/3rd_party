#ifndef TRACY_ENABLE
#define TRACY_ENABLE
#endif

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "tracy/Tracy.hpp"

// Function to demonstrate time measurement
void SlowFunction() {
  // ZoneScoped measures the lifetime of the current scope
  ZoneScoped;

  // Custom name for a specific zone
  ZoneName("MySlowProcess", 13);

  std::cout << "Slow function is running..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void FastFunction() {
  // A zone with a simple name
  ZoneScopedN("QuickJob");
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main() {
  std::cout << "Tracy client starting..." << std::endl;
  std::cout << "Run Tracy Profiler (server):" << std::endl;
  std::cout << "  It can be downloaded from https://github.com/wolfpld/tracy/releases" << std::endl;

  // Set a custom name for the thread to identify it easily in the Tracy UI
  tracy::SetThreadName("Main Thread");

  std::cout << "Tracy client started. Connect with Tracy Profiler (server) now!" << std::endl;

  for (int i = 0; i < 50; ++i) {
    // FrameMark signals the end of a frame.
    // Even in non-graphical apps, it helps group data into logical iterations.
    FrameMark;

    SlowFunction();
    FastFunction();

    // Send a text message directly to the Tracy timeline
    TracyMessageL("Iteration step complete");
  }

  std::cout << "Processing finished." << std::endl;
  return 0;
}