// Simple logging function. Logs thread id before each line. USAGE:
/*
#include "debug_log/DebugLog.h"
#define DISABLE_DEBUG_LOG // disable logging
#define DEBUG_LOG_PREFIX "[APP] " // override prefix for this file
debugLog() << "line" << msg << std::endl;
*/

#pragma once

#include <iostream>
#include <thread>

#ifndef DEBUG_LOG_PREFIX
#define DEBUG_LOG_PREFIX "[DEBUG] "
#endif

struct NullStream {
  template <typename T>
  NullStream& operator<<(const T&) { return *this; }

  NullStream& operator<<(std::ostream& (*)(std::ostream&)) { return *this; }
};

#ifdef DISABLE_DEBUG_LOG
#define debugLog() \
  if (true) {      \
  } else           \
    NullStream()
#else
#define debugLog() std::cout << DEBUG_LOG_PREFIX << "[" << std::this_thread::get_id() << "] "
#endif