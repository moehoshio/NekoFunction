# Utilities.hpp

A modern C++ utility library that provides wrapped implementations of practical functions.  
Includes utilities such as operators, string processing, hash calculation, time handling, Base64 encoding, random number generation, and UUID tools.

## Features

- Easy to use
- Modern C++
- Header-only
- Cross-platform

## Functionalities

- 🎯 **Operators** - Pipe operator, fallback operator, optional chaining
- 🔧 **String tools** - Case conversion, path processing, file extension checking
- 🔒 **Hash calculation** - MD5, SHA1, SHA256, SHA512 support (requires OpenSSL)
- ⏰ **Time handling** - ISO 8601 parsing, UTC/local time conversion
- 🔢 **Base64 encoding/decoding** - Standard Base64 encoding and decoding
- 🎲 **Random number generation** - Random strings, numbers, hexadecimal values
- 🆔 **UUID generation** - Support for UUID v3 and v4

## Quick Start

1. Clone the repository locally

```bash
git clone https://github.com/moehoshio/utilities.hpp.git && cd utilities.hpp
```

2. Copy the entire include directory to your include path

3. Include the header file in your code

```cpp
#include "neko/function/utilities.hpp"
```

4. Use the utilities

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
- [Complete Example](#complete-example)
- [License](#license)

### Hash Support (Optional)

Hash support will be automatically enabled if OpenSSL is installed.  
Please ensure OpenSSL headers are in your include path and link the library.

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

using namespace neko::util::hash;

// String hash
auto md5_hash = hash("hello", Algorithm::md5);
auto sha256_hash = hash("hello", Algorithm::sha256);

// File hash
auto file_hash = hashFile("document.txt", Algorithm::sha1);

// Algorithm mapping
auto algo = mapAlgorithm("sha256"); // Algorithm::sha256
auto name = mapAlgorithm(Algorithm::md5); // "md5"
```

## UUID

```cpp
using namespace neko::util::uuid;

// UUID v4 (fully random)
auto uuid4 = uuidV4(); // "550e8400-e29b-41d4-a716-446655440000"

#if defined(NEKO_ENABLE_HASH_SUPPORT)
// UUID v3 (requires hash support, name-based)
auto uuid3 = uuidV3("example.com"); // Based on default namespace
auto uuid3_custom = uuidV3("example.com", "custom-namespace-uuid");
#endif
```

## Complete Example

```cpp
#include <openssl/sha.h>
#include "neko/function/utilities.hpp"
#include <iostream>

using namespace neko::ops::pipe;
using namespace neko::util;

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
    std::time_t now = getUtcNow(); 
    auto local_time = time::getLocalTimeString();
    auto utc_time = getUtcZTimeString();
    if ( logic::allTrue(local_time, utc_time) ) {
        std::cout << "Current timestamp: " << now << std::endl;
        std::cout << "Current local time: " << *local_time << std::endl;
        std::cout << "Current UTC time: " << *utc_time << std::endl;
    }
    
    // Random numbers and UUID
    auto random_id = uuid::uuidV4();
    auto random_token = random::generateRandomString(16);
    
    std::cout << "UUID: " << random_id << std::endl;
    std::cout << "Token: " << random_token << std::endl;
    
    // Hash calculation
    auto hash_value = hash::hash("sensitive_data", hash::Algorithm::sha256);
    std::cout << "SHA256: " << hash_value << std::endl;
    
    return 0;
}
```

## LICENSE

MIT OR Apache-2.0