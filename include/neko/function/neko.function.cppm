/**
 * @file neko.function.cppm
 * @brief C++20 module interface for NekoFunction
 * @details This module exports base NekoFunction functionality by wrapping the header files.
 *          The original headers are still available for traditional include-based usage.
 */

module;

// ====================
// = Standard Library =
// ====================

#if defined(__cpp_lib_modules) && (__cpp_lib_modules >= 202207L)
import std;
import std.compat;
#else
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <optional>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cstring>
#endif

// ====================
// === Hash Support ===
// ====================
#if defined(NEKO_FUNCTION_ENABLE_HASH)

// Include OpenSSL headers for hash functions
#if defined(NEKO_IMPORT_OPENSSL)
#include <openssl/evp.h>

#else
#undef NEKO_FUNCTION_ENABLE_HASH // If no supported hash functions are available, undefine the macro
#endif

#endif // NEKO_FUNCTION_ENABLE_HASH

import neko.schema;

export module neko.function;

// Control header files to not import dependencies (dependencies are declared and imported by the cppm)
#define NEKO_FUNCTION_ENABLE_MODULE true

export {
    #include "pattern.hpp"
    #include "utilities.hpp"
    #include "detectFileType.hpp"
    #include "hash.hpp"
    #include "uuid.hpp"
}
