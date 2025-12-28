#pragma once

#include <iostream>

// ---

// Simple logging function. Logs thread id before each line.
// To enable/disable logging, use `DISABLE_DEBUG_LOG` macro.
// USAGE:
// debugLog() << "line" << msg << std::endl;
inline std::ostream& debugLog() {
#ifdef DISABLE_DEBUG_LOG
  static std::ostream nullStream(nullptr);
  return nullStream;
#else
  std::cout << "[" << std::this_thread::get_id() << "] ";
  return std::cout;
#endif
}
