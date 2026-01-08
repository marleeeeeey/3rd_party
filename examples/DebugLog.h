// Simple logging function. Logs thread id before each line. USAGE:
/*
#include "debug_log/DebugLog.h"
#define DEBUG_LOG_DISABLE_DEBUG_LEVEL // disable logging at DEBUG level
#define DEBUG_LOG_DISABLE_VERBOSE_LEVEL // disable logging at VERBOSE level
#define DEBUG_LOG_USER_PREFIX "[APP]" // override prefix for this file
debugLog() << "line" << msg << std::endl;
*/

#pragma once

#include <iostream>
#include <thread>

#ifndef DEBUG_LOG_USER_PREFIX
#define DEBUG_LOG_USER_PREFIX ""
#endif

struct NullStream {
  template <typename T>
  NullStream& operator<<(const T&) { return *this; }

  NullStream& operator<<(std::ostream& (*)(std::ostream&)) { return *this; }
};

// clang-format off
#ifdef DEBUG_LOG_DISABLE_DEBUG_LEVEL
#define debugLog() if (true) { } else NullStream()
#else
#define debugLog() std::cout << DEBUG_LOG_USER_PREFIX << "[DEBUG]" << "[" << std::this_thread::get_id() << "] "
#endif

#ifdef DEBUG_LOG_DISABLE_VERBOSE_LEVEL
#define verboseLog() if (true) { } else NullStream()
#else
#define verboseLog() std::cout << DEBUG_LOG_USER_PREFIX << "[VERBO]" << "[" << std::this_thread::get_id() << "] "
#endif
// clang-format on