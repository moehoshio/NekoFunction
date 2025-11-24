# NekoFunction

A comprehensive modern C++ utility library that provides practical functions for common programming tasks.  
NekoFunction integrates multiple functional modules including utilities, archive management and file type detection.

[![License](https://img.shields.io/badge/License-MIT%20OR%20Apache--2.0-blue.svg)](LICENSE)
![Require](https://img.shields.io/badge/%20Require%20-%3E=%20C++%2020-orange.svg)
[![CMake](https://img.shields.io/badge/CMake-3.14+-green.svg)](https://cmake.org/)
![Module Support](https://img.shields.io/badge/Modules-C%2B%2B20-blueviolet.svg)

## Features

- Easy-to-use, simple and intuitive API
- Utilities for common programming tasks (string , time, iso8601, base64 ,random, validation)
- Hash functions (MD5, SHA1, SHA256, SHA512)
- Archive management with ZIP support and encryption
- Cross-platform compatibility
- Modern C++20 - Module support

## Core Modules

- 🎯 **Utilities** - Operators, string processing, memory management, time handling, Base64, random, validation
- 🔒 **Hash** - Cryptographic hash functions (MD5, SHA1, SHA256, SHA512) - requires OpenSSL
- 🆔 **UUID** - UUID generation (v4 always available, v3 requires hash support)
- 📦 **Archive Management** - ZIP archive creation and extraction with encryption support
- 🔍 **File Type Detection** - Automatic file type detection based on magic numbers and extensions

## Functionalities

### Utilities Module

- 🎯 **Operators** - Pipe operator, fallback operator, optional chaining
- 🔧 **String tools** - Case conversion, path processing, file extension checking
- 🧠 **Memory utilities** - Array creation, value copying, smart pointer helpers
- 🔄 **Lambda utilities** - Common lambda functions for functional programming
- 🧮 **Logic and Math** - Boolean operations, sum, product calculations
- ⏰ **Time handling** - ISO 8601 parsing, UTC/local time conversion
- 🔢 **Base64 encoding/decoding** - Standard Base64 encoding and decoding
- 🎲 **Random utilities** - Random strings, numbers, hexadecimal values
- ✅ **Validation utilities** - URL validation, proxy address validation, resolution matching

### Hash Module (Requires OpenSSL)

- 🔒 **Cryptographic Hash Functions** - MD5, SHA1, SHA256, SHA512
- 📄 **String and File Hashing** - Compute hash for strings and files
- 🔧 **Algorithm Mapping** - Convert between algorithm names and enum values

### UUID Module

- 🆔 **UUID v4** - Random UUID generation (always available, no dependencies)
- 🔐 **UUID v3** - Name-based UUID generation (requires hash support/OpenSSL)

### Archive Management Module

- 📦 **ZIP Archive Support** - Create and extract ZIP archives
- 🔐 **Encryption Support** - AES256 and ZipCrypto encryption
- ⚙️ **Compression Levels** - Configurable compression from none to ultra
- 🎯 **Pattern-based Selection** - Include/exclude files using patterns, wildcards, and regex

### File Type Detection Module

- 🔍 **Magic Number Detection** - Identify file types by examining file headers
- 📁 **Extension Matching** - Validate file types based on extensions
- 🎯 **Dual Validation** - Combine magic number and extension for accurate detection
- 📋 **Extensive Format Support** - Support for common file formats (images, archives, documents, etc.)

## Requirements

- C++20 or higher compatible compiler
- CMake 3.14 or higher
- Git

## Quick Start

Configure: [CMake](#cmake) | [vcpkg](#vcpkg) | [Conan](#conan) | [Static Linking](#static-linking) | [Test](#test)

Usage:

- [Core Utilities](#core-utilities)
  - [Operators](#operators)
    - [Pipe](#pipe-)
    - [Fallback](#fallback-operator-)
    - [Optional Chaining](#optional-chaining-)
    - [Ranges](#ranges)
  - [String](#string-utilities)
    - [Basic Operations](#basic-string-operations)
    - [Path and File Handling](#path-and-file-handling)
  - [Memory](#memory-utilities)
  - [Lambda](#lambda)
  - [Logic and Math](#logic-and-math)
  - [Time](#time)
    - [Basic Operations](#basic-time-operations)
    - [ISO 8601 Parsing](#iso-8601-parsing)
  - [Base64](#base64-encodingdecoding)
  - [Random](#random-utilities)
  - [Validation](#validation-utilities)
- [Hash](#hash-calculation-requires-openssl)
- [UUID](#uuid)
- [Archive Management](#archive-management)
  - [ZIP Archive Operations](#zip-archive-operations)
  - [Configuration Options](#configuration-options)
- [File Type Detection](#file-type-detection)
  - [Magic Number Detection](#magic-number-detection)
  - [Extension-based Detection](#extension-based-detection)
- [Pattern Matching](#pattern-matching)
  - [Wildcard Patterns](#wildcard-patterns)
  - [Regular Expression Patterns](#regular-expression-patterns)
  - [Path Matching](#path-matching)
- [Complete Example](#complete-example)

### CMake

1. Using CMake's `FetchContent` to include NekoFunction in your project:

```cmake
include(FetchContent)

# Add NekoFunction to your CMake project
FetchContent_Declare(
    NekoFunction
    GIT_REPOSITORY https://github.com/moehoshio/NekoFunction.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(NekoFunction)

target_link_libraries(your_target PRIVATE Neko::Function)
```

2. Include the header files in your code

```cpp
#include <neko/function/utilities.hpp>
```

#### C++20 Module Support

NekoFunction supports C++20 modules. To use NekoFunction as a module, ensure your compiler supports C++20 modules and configure your project accordingly.

```cmake
...
# Add NekoFunction to your CMake project
FetchContent_Declare(
    NekoFunction
    GIT_REPOSITORY https://github.com/moehoshio/NekoFunction.git
    GIT_TAG        main
)
 

# Set Variables Before Building
set(NEKO_FUNCTION_ENABLE_MODULE ON CACHE BOOL "Enable Module support" FORCE)
FetchContent_MakeAvailable(NekoFunction)

target_link_libraries(your_target PRIVATE Neko::Function::Module)
```

Import the module in your C++ code:

```cpp
import neko.function;
```

#### Archive Management (Optional)

To enable Archive support, set the `NEKO_FUNCTION_ENABLE_ARCHIVE` variable to `ON` (default), and install [minizip-ng](https://github.com/nmoinvaz/minizip-ng). Then configure CMake to search for your minizip-ng installation path.

Set the `CMAKE_PREFIX_PATH` or `NEKO_FUNCTION_LIBRARY_PATH` variable to your minizip-ng installation directory.

```shell
cmake -D NEKO_FUNCTION_ENABLE_ARCHIVE=ON -D NEKO_FUNCTION_LIBRARY_PATH=/path/to/your/minizip-ng -B ./build -S .
```

or

```cmake 
...
# Add NekoFunction to your CMake project
FetchContent_Declare(
    NekoFunction
    GIT_REPOSITORY https://github.com/moehoshio/NekoFunction.git
    GIT_TAG        main
)
# Set Variables Before Building
find_package(minizip-ng REQUIRED)
set(NEKO_FUNCTION_ENABLE_ARCHIVE ON CACHE BOOL "Enable Archive support" FORCE)
FetchContent_MakeAvailable(NekoFunction)
...
```

If minizip-ng is found, you should see output similar to:

```shell
-- Dependency summary:
    ...
--   - minizip-ng support: TRUE version: 4.0.1
--   - Archive support enabled: TRUE via minizip-ng
    ...
```

#### Hash Support (Optional)

To enable Hash support, set the `NEKO_FUNCTION_ENABLE_HASH` variable to `ON` (default), and install [OpenSSL](https://www.openssl.org/). Then configure CMake to search for your OpenSSL installation path.

```shell
cmake -D NEKO_FUNCTION_ENABLE_HASH=ON -D NEKO_FUNCTION_LIBRARY_PATH=/path/to/your/openssl -B ./build -S .
```

or

```cmake
...
# Add NekoFunction to your CMake project
FetchContent_Declare(
    NekoFunction
    GIT_REPOSITORY https://github.com/moehoshio/NekoFunction.git
    GIT_TAG        main
)
# Set Variables Before Building
find_package(OpenSSL REQUIRED)
set(NEKO_FUNCTION_ENABLE_HASH ON CACHE BOOL "Enable Hash support" FORCE)
FetchContent_MakeAvailable(NekoFunction)
...
```

if OpenSSL is found, you should see output similar to:

```shell
-- Dependency summary:
    ...
--   - OpenSSL support: TRUE version: 3.0.10
--   - Hash support enabled: TRUE via OpenSSL
    ...
```

#### Static Linking

If you want to use static linking, make sure all libraries (OpenSSL, minizip-ng..) are built as static libraries. (NekoFunction itself is always a static library)
Enable the static linking option in your CMake configuration:

```bash
cmake -B ./build . -DNEKO_FUNCTION_STATIC_LINK=ON -DNEKO_FUNCTION_LIBRARY_PATH="/path/to/x64-windows-static" -S .
```

### Vcpkg

To install NekoFunction using vcpkg, run the following command:

```bash
vcpkg install neko-function
```

Or add it to your `vcpkg.json`:

```json
{
  "dependencies": ["neko-function"]
}
```

Then in your CMakeLists.txt:

```cmake
find_package(NekoFunction CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Neko::Function)
```

When configuring your project, specify the vcpkg toolchain file:

```shell
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/vcpkg/installed/x64-windows
cmake --build build --config Debug
```

Archive support via features:

```bash
vcpkg install neko-function[archive]
```

Hash support via features:

```bash
vcpkg install neko-function[hash]
```

All Enabled:

```bash
vcpkg install neko-function[archive,hash,core]
```

Note: Installing via vcpkg does not support modules.

### Conan

To install NekoFunction using Conan, add the following to your `conanfile.txt`:

```ini
[requires]
neko-function/[*]

[generators]
CMakeDeps
CMakeToolchain
```

Or use it in your `conanfile.py`:

```python
from conan import ConanFile

class YourProject(ConanFile):
    requires = "neko-function/[*]"
    generators = "CMakeDeps", "CMakeToolchain"
```

Then install and use:

```shell
conan install . --build=missing
cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build
```

In your CMakeLists.txt:

```cmake
find_package(NekoFunction CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Neko::Function)
```

#### Conan with C++20 Module Support

To enable C++20 module support with Conan, use the `enable_module` option:

```shell
conan install . --build=missing -o neko-function/[*]:enable_module=True
```

Or specify it in your `conanfile.txt`:

```ini
[requires]
neko-function/[*]

[options]
neko-function/*:enable_module=True

[generators]
CMakeDeps
CMakeToolchain
```

Or in your `conanfile.py`:

```python
from conan import ConanFile

class YourProject(ConanFile):
    requires = "neko-function/[*]"
    generators = "CMakeDeps", "CMakeToolchain"
    
    def configure(self):
        self.options["neko-function"].enable_module = True
```

Then link against the module target in your CMakeLists.txt:

```cmake
find_package(NekoFunction CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Neko::Function::Module)
```

## Core Utilities

The utilities module provides the foundation of NekoFunction, including functional programming operators, string manipulation, memory management, and various utility functions.

```cpp
#include <neko/function/utilities.hpp>
```

## Operators

### pipe (`|`)

Pipe operation that passes the result of the previous expression as input to the next expression.

```cpp
using namespace neko::ops::pipe;

// Basic usage
auto result = 5 | [](int x) { return x * 2; }; // result = 10

// Chaining operations
auto toUpper = [](std::string s) { return neko::util::string::toUpper<std::string>(s); };

auto result = "hello" 
    | neko::util::plusDoubleQuote 
    | toUpper; // result = "HELLO"
```

### Fallback operator (`||`)

Fallback operator that uses the result of the next expression when the previous expression's result is empty.
Note: Requires function return result to be convertible to bool and dereferenceable with *.

```cpp
using namespace neko::ops::fallback;

auto try_parse = [](std::string s) -> std::optional<int> {
    try { return std::stoi(s); }
    catch(...) { return std::nullopt; }
};
auto default_value = [](std::string s) -> int { return -1; };

auto parser = try_parse || default_value;
auto result = parser("invalid"); // result = -1
```

### Optional chaining (`>>=`)

Optional chaining that continuously passes expression results to the next expression when the result is non-empty.
If the result is empty, it short-circuits.
Note: Requires function return result to be convertible to bool and dereferenceable with * (dereferences value to next expression).

```cpp
using namespace neko::ops::logic;

std::optional<int> parse(std::string s) { /* ... */ }
std::optional<double> to_double(int x) { return x * 1.5; }
std::optional<std::string> to_string(double x) { return std::to_string(x); };

auto result = parse("42") >>= to_double >>= to_string; // std::optional<std::string> "63.00"
```

### Ranges

```cpp
using namespace neko::ops::ranges;

// Apply function to range
auto sum = {1, 2, 3, 4} | [](const auto& vec) {
    return std::accumulate(vec.begin(), vec.end(), 0);
}; // sum = 10

using namespace neko::ops::foreach;

// Execute operation on each element
{1, 2, 3} | [](int x) { std::cout << x << " "; }; // Output: 1 2 3
```

note : This conflicts with the syntax of std::ranges::views.

## String Utilities

### Basic String Operations

```cpp
using namespace neko::util::string;

// Add quotes
auto quoted = wrapWith("hello", "\"", "\""); // "hello"
auto quoted2 = wrapWith("world", "'"); // 'world'

// Case conversion
auto lower = toLower<std::string>("HELLO"); // "hello"
auto upper = toUpper<std::string>("world"); // "WORLD"
```

### Path and File Handling

```cpp
// Get extension
auto ext = getExtensionName("document.txT"); // "txt"
auto ext2 = getExtensionName("archive.TAR.GZ", true); // "GZ" (case sensitive)

// Check extension
bool isText = matchExtensionName("file.txt", "txT"); // true
bool isDoc = matchExtensionNames("doc.pdF", {"txt", "md", "pdf"}); // true

// Path normalization
auto unixPath = convertToUnixPath("C:\\Users\\Name\\file.txt"); // "C:/Users/Name/file.txt"
```

## Memory Utilities

```cpp
using namespace neko::util::memory;

// Create array
auto arr = makeArray(1, 2, 3, 4); // std::array<int, 4>

// Copy value
auto copied = copy(42); // 42
int* ptr = new int(100);
auto deep_copied_ptr = copy(ptr); // Deep copy pointer
```

## Lambda

Lambda wrappers for common utilities, so they can be used with Ops

```cpp
using namespace neko::util::lambda;

// Common lambdas
auto quoted = "hello" | plusDoubleQuote; // "hello"
auto path = "C:\\test" | unifiedPath; // "C:/test"
auto shared = 42 | makeShared; // std::shared_ptr<int>
auto copied = some_value | copy;
```

## Logic and Math

```cpp
using namespace neko::util::logic;

// Boolean conversion
auto str = boolTo<std::string>(true, "yes", "no"); // "yes"

// Logic operations
bool all = allTrue(true, true, false); // false
bool any = anyTrue(false, false, true); // true

using namespace neko::util::math;

// Math operations
auto total = sum(1, 2, 3, 4, 5); // 15
auto product_val = product(2, 3, 4); // 24
```

## Time

### Basic Time Operations

```cpp
using namespace neko::util::time;

// Get current time
std::time_t now = getUtcNow(); // Current UTC integer timestamp
std::string nowStr = timeToString( getUtcNow() ); // String timestamp "1680000"

// Format time
auto local_time = getLocalTimeString(); // "2024-01-15-14-30-45"
auto utc_time = getUtcZTimeString(); // "2024-01-15T14:30:45Z"
```

### ISO 8601 Parsing

```cpp
// Parse ISO 8601 format
auto utc_time = parseToUTCTime("2024-01-15T14:30:45Z"); // 1680000
auto utc_str = parseToUTCTimeString("2024-01-15T14:30:45+08:00"); // 168000
auto local_str = parseToLocalTimeString("2024-01-15T14:30:45Z"); // "2024-01-15-20-30-45" Local time
```

## Base64 Encoding/Decoding

```cpp
using namespace neko::util::base64;

// Encoding
auto encoded = base64Encode("Hello, World!"); // "SGVsbG8sIFdvcmxkIQ=="

// Decoding
auto decoded = base64Decode("SGVsbG8sIFdvcmxkIQ=="); // "Hello, World!"
```

## Random Utilities

```cpp
using namespace neko::util::random;

// Random hexadecimal string
auto hex = randomHex(8); // "a1b2c3d4"

// Random number, 3-5 digit random number
auto num = randomNDigitNumber(3, 5); // 999

// Random string
auto str = generateRandomString(10); // 10 character random string
auto custom = generateRandomString(8, "ABCDEF0123456789-/.*"); // Custom character set
```

## Validation Utilities

```cpp
using namespace neko::util::check;

// URL validation
bool valid_url = isUrl("https://www.example.com"); // true

// Proxy address validation
bool valid_proxy = isProxyAddress("socks5://127.0.0.1:1080"); // true

// Resolution matching
auto res = matchResolution("Set to 1920x1080 resolution");
if (res) {
    std::cout << "Width: " << res.width << ", Height: " << res.height; // Width: 1920, Height: 1080
}
```

## Hash Calculation (Requires OpenSSL)

```cpp
#include <neko/function/hash.hpp>

using namespace neko::util::hash;

// Compute string hash digest
auto md5_digest = digest("hello", Algorithm::md5);
auto sha256_digest = digest("hello", Algorithm::sha256);

// Compute file hash digest
auto file_digest = digestFile("document.txt", Algorithm::sha1);

// Algorithm mapping
auto algo = mapAlgorithm("sha256"); // Algorithm::sha256
auto name = mapAlgorithm(Algorithm::md5); // "md5"
```

## UUID

```cpp
#include <neko/function/uuid.hpp>

using namespace neko::util::uuid;

// UUID v4 (fully random, always available)
auto uuid4 = uuidV4(); // "550e8400-e29b-41d4-a716-446655440000"

#if defined(NEKO_IMPORT_OPENSSL)
// UUID v3 (requires hash support/OpenSSL, name-based)
auto uuid3 = uuidV3("example.com"); // Based on default namespace
auto uuid3_custom = uuidV3("example.com", "custom-namespace-uuid");
#endif
```

## Archive Management

The archive management module provides comprehensive support for creating and extracting ZIP archives with advanced features like encryption and pattern-based file selection.

### ZIP Archive Operations

```cpp
#include <neko/function/archive.hpp>

using namespace neko::archive;

// Create archive configuration
CreateConfig createConfig;
createConfig.outputArchivePath = "backup.zip";
createConfig.password = "secret123";
createConfig.compressionLevel = CompressionLevel::maximum;
createConfig.encryption = ZipEncryption::AES256;

// Include specific files and patterns
createConfig.inputPaths = {
    "/path/to/documents/",     // Include entire directory
    "*.txt",                   // Include all text files
    "logs/*.log",              // Include log files in logs directory
    "^config/.*\\.json$"       // Regex: all JSON files in config directory
};

// Exclude patterns
createConfig.excludePaths = {
    "temp/",                   // Exclude temp directory
    "*.tmp"                    // Exclude temporary files
};

// Create the archive
zip::create(createConfig);

// Extract archive configuration
ExtractConfig extractConfig;
extractConfig.inputArchivePath = "backup.zip";
extractConfig.destDir = "/path/to/extract/";
extractConfig.password = "secret123";
extractConfig.overwriteExisting = true;

// Selectively extract files
extractConfig.includePaths = {
    "documents/",              // Extract only documents folder
    "*.pdf"                    // Extract only PDF files
};

// Extract the archive
zip::extract(extractConfig);

// Check if file is a ZIP archive
bool isZip = zip::isZipFile("backup.zip");
```

### Configuration Options

- **Compression Levels**: `none`, `fast`, `normal`, `maximum`, `ultra`
- **Encryption**: `ZipCrypto`, `AES256`
- **Pattern Support**: File names, relative paths, absolute paths, wildcards, regex
- **Directory Handling**: Include/exclude entire directories
- **Selective Operations**: Choose specific files to include or exclude

## File Type Detection

The file type detection module provides accurate file type identification using both magic numbers (file signatures) and file extensions.

### Magic Number Detection

```cpp
#include <neko/function/detectFileType.hpp>

using namespace neko::util::detect;

// Detect file type from file path
auto fileType = detectFileType("/path/to/document.pdf");
if (fileType) {
    std::cout << "Detected type: " << *fileType << std::endl;  // "PDF"
}

// Check if file is of specific type
bool isPdf = isFileType("/path/to/document.pdf", "PDF");
bool isImage = isFileType("/path/to/photo.jpg", "JPEG");

// Detect from file content (when extension is misleading)
auto actualType = detectFileType("/path/to/renamed_file.txt");  // Might detect "PNG" if it's actually an image
```

### Extension-based Detection

The detection system uses a dual approach:

1. **Magic Number Scanning**: Reads file headers to identify true file type
2. **Extension Validation**: Verifies that the detected type matches expected extensions
3. **Combined Validation**: Ensures both magic number and extension are consistent

Supported formats include:

- **Images**: PNG, JPEG, GIF, BMP, TIFF, WebP
- **Archives**: ZIP, RAR, 7Z, TAR, GZIP
- **Documents**: PDF, DOC, DOCX, XLS, XLSX, PPT, PPTX
- **Executables**: EXE, DLL, SO, DMG
- **Media**: MP3, MP4, AVI, MOV, WAV
- **And many more...**

## Pattern Matching

The pattern matching module provides sophisticated pattern matching capabilities for file paths and names, supporting wildcards, regular expressions, and various path matching strategies.

### Wildcard Patterns

```cpp
#include <neko/function/pattern.hpp>

using namespace neko::util::pattern;

// Basic wildcard matching
bool matches = matchPattern("document.txt", "*.txt");        // true
bool matches2 = matchPattern("logs/app.log", "logs/*.log");  // true
bool matches3 = matchPattern("src/main.cpp", "src/*");       // true

// Check pattern types
bool isWild = containsWildcard("*.txt");      // true
bool isExt = isExtensionPattern(".pdf");      // true
bool isDir = isPatternDir("logs/");           // true
```

### Regular Expression Patterns

```cpp
// Advanced regex patterns
std::vector<std::string> patterns = {
    "^logs/.*\\.log$",          // Log files in logs directory
    "^config/.*\\.json$",       // JSON files in config directory  
    "^src/.*\\.(cpp|hpp)$"      // C++ source files in src directory
};

std::vector<std::string> testPaths = {
    "logs/application.log",
    "config/settings.json", 
    "src/main.cpp",
    "docs/readme.txt"
};

// Test multiple patterns
for (const auto& path : testPaths) {
    bool matched = matchPatterns(path, patterns);
    if (matched) {
        std::cout << path << " matches one of the patterns" << std::endl;
    }
}
```

### Path Matching

```cpp
// Different path matching strategies

// 1. Absolute path matching
bool exactMatch = matchAbsolutePath("/home/user/docs/file.txt", "/home/user/docs/file.txt");

// 2. Directory containment
bool inDirectory = matchAbsoluteDirectory("/home/user/docs/file.txt", "/home/user/docs/");

// 3. Relative path ending
bool endsWithPath = matchRelativePath("/home/user/docs/project/src/main.cpp", "src/main.cpp");

// 4. Directory name matching
bool hasDirectory = matchDirectoryName("/project/logs/app.log", "logs");

// 5. Extension matching  
bool isTextFile = matchExtension("document.txt", ".txt");

// Combined pattern matching with multiple rules
std::vector<std::string> includePatterns = {
    "/home/user/docs/",         // Absolute directory
    "*.cpp",                    // All C++ files
    "logs/",                    // Any logs directory
    "^config/.*\\.json$"        // JSON files in config (regex)
};

bool shouldInclude = matchPatterns("/home/user/project/src/main.cpp", includePatterns);
```

## Complete Example

```cpp
#include <neko/function/utilities.hpp>
#include <neko/function/archive.hpp>
#include <neko/function/detectFileType.hpp>
#include <neko/function/pattern.hpp>
#include <iostream>

using namespace neko::ops::pipe;
using namespace neko::util;
using namespace neko::archive;

int main() {
    // Functional style data processing
    auto toUpper = [](std::string s) { return string::toUpper<std::string>(s); };

    auto result = "Hello World" 
        | toUpper
        | lambda::plusDoubleQuote
        | [](const std::string& s) {
            std::cout << "Processing result: " << s << std::endl;
            return s;
        };
    
    // Time handling
    std::time_t now = time::getUtcNow(); 
    auto local_time = time::getLocalTimeString();
    auto utc_time = time::getUtcZTimeString();
    if (logic::allTrue(local_time, utc_time)) {
        std::cout << "Current timestamp: " << now << std::endl;
        std::cout << "Current local time: " << *local_time << std::endl;
        std::cout << "Current UTC time: " << *utc_time << std::endl;
    }
    
    // Random numbers and UUID
    auto random_id = uuid::uuidV4();
    auto random_token = random::generateRandomString(16);
    
    std::cout << "UUID: " << random_id << std::endl;
    std::cout << "Token: " << random_token << std::endl;
    
    // Hash calculation (if OpenSSL is available)
    #ifdef NEKO_IMPORT_OPENSSL
    auto hash_value = hash::digest("sensitive_data", hash::Algorithm::sha256);
    std::cout << "SHA256: " << hash_value << std::endl;
    #endif
    
    // File type detection
    auto fileType = detect::detectFileType("document.pdf");
    if (fileType) {
        std::cout << "File type detected: " << *fileType << std::endl;
    }
    
    // Pattern matching
    std::vector<std::string> patterns = {"*.cpp", "*.hpp", "logs/*.log"};
    bool matches = pattern::matchPatterns("src/main.cpp", patterns);
    std::cout << "Pattern match result: " << (matches ? "true" : "false") << std::endl;
    
    // Archive creation
    CreateConfig archiveConfig;
    archiveConfig.outputArchivePath = "backup.zip";
    archiveConfig.compressionLevel = CompressionLevel::maximum;
    archiveConfig.inputPaths = {"src/", "*.txt", "docs/"};
    archiveConfig.excludePaths = {"*.tmp", "temp/"};
    
    try {
        zip::create(archiveConfig);
        std::cout << "Archive created successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Archive creation failed: " << e.what() << std::endl;
    }
    
    return 0;
}
```

## Test

You can run the tests to verify that everything is working correctly.

If you haven't configured the build yet, please run:

```shell
cmake -D NEKO_FUNCTION_BUILD_TESTS=ON -D NEKO_FUNCTION_AUTO_FETCH_DEPS=ON -D NEKO_FUNCTION_LIBRARY_PATH=<path_to_openssl_and_minizip_ng> -B ./build -S .
```

Now, you can build the test files with the following command:

```shell
cmake --build ./build --config Debug
```

Then, you can run the tests with the following commands:

```shell
cd ./build && ctest --output-on-failure
```

If everything is set up correctly, you should see output similar to the following:

```shell


  Test project /path/to/NekoFunction/build
      Start 1: NekoFunction_tests
  1/1 Test #1: NekoFunction_tests ...............   Passed    0.21 sec

  100% tests passed, 0 tests failed out of 1

  Total Test time (real) =   0.21 sec
```

### Disable Tests

If you want to disable building and running tests, you can set the following CMake option when configuring your project:

```shell
cmake -B ./build -DNEKO_FUNCTION_BUILD_TESTS=OFF -S .
```

or

```cmake
...
# Set Variables Before Building
set(NEKO_FUNCTION_BUILD_TESTS OFF CACHE BOOL "Disable building tests" FORCE)
FetchContent_MakeAvailable(NekoFunction)
```

This will skip test targets during the build process.

## License

[License](LICENSE) MIT OR Apache-2.0

## See More

- [NekoNet](https://github.com/moehoshio/NekoNet): A modern , easy-to-use C++20 networking library via libcurl.
- [NekoLog](https://github.com/moehoshio/NekoLog): An easy-to-use, modern, lightweight, and efficient C++20 logging library.
- [NekoEvent](https://github.com/moehoshio/NekoEvent): A modern easy to use type-safe and high-performance event handling system for C++.
- [NekoSchema](https://github.com/moehoshio/NekoSchema): A lightweight, header-only C++20 schema library.
- [NekoSystem](https://github.com/moehoshio/NekoSystem): A modern C++20 cross-platform system utility library.
- [NekoFunction](https://github.com/moehoshio/NekoFunction): A comprehensive modern C++ utility library that provides practical functions for common programming tasks.
- [NekoThreadPool](https://github.com/moehoshio/NekoThreadPool): An easy to use and efficient C++ 20 thread pool that supports priorities and submission to specific threads.