/**
 * @file test.cpp
 * @brief Minimal integration test for NekoFunction via vcpkg
 * @details Tests basic functionality to ensure the package is properly installed
 */

#include <neko/function/utilities.hpp>
#include <neko/function/pattern.hpp>
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== NekoFunction vcpkg Integration Test ===" << std::endl;
    
    // Test 1: String utilities
    {
        using namespace neko::util::string;
        auto quoted = wrapWith("hello", "\"", "\"");
        assert(quoted == "\"hello\"");
        std::cout << "✓ String utilities work" << std::endl;
    }
    
    // Test 2: String case conversion
    {
        using namespace neko::util::string;
        auto lower = toLower<std::string>("HELLO");
        assert(lower == "hello");
        auto upper = toUpper<std::string>("world");
        assert(upper == "WORLD");
        std::cout << "✓ Case conversion works" << std::endl;
    }
    
    // Test 3: Base64 encoding/decoding
    {
        using namespace neko::util::base64;
        auto encoded = base64Encode("Test");
        auto decoded = base64Decode(encoded);
        assert(decoded == "Test");
        std::cout << "✓ Base64 encoding/decoding works" << std::endl;
    }
    
    // Test 4: Math utilities
    {
        using namespace neko::util::math;
        auto result = sum(1, 2, 3, 4, 5);
        assert(result == 15);
        std::cout << "✓ Math utilities work" << std::endl;
    }
    
    // Test 5: Pattern matching
    {
        using namespace neko::util::pattern;
        assert(containsWildcard("*.txt") == true);
        assert(containsWildcard("file.txt") == false);
        assert(matchWildcardPattern("test.txt", "*.txt") == true);
        std::cout << "✓ Pattern matching works" << std::endl;
    }
    
    // Test 6: Pipe operator
    {
        using namespace neko::ops::pipe;
        auto doubleValue = [](int x) { return x * 2; };
        auto result = 5 | doubleValue;
        assert(result == 10);
        std::cout << "✓ Pipe operator works" << std::endl;
    }
    
    // Test 7: Logic utilities
    {
        using namespace neko::util::logic;
        assert(allTrue(true, true, true) == true);
        assert(allTrue(true, false, true) == false);
        assert(anyTrue(false, true, false) == true);
        std::cout << "✓ Logic utilities work" << std::endl;
    }
    
    // Test 8: UUID generation
    {
        using namespace neko::util::uuid;
        auto uuid = uuidV4();
        assert(uuid.length() == 36);
        assert(uuid[8] == '-');
        assert(uuid[13] == '-');
        std::cout << "✓ UUID generation works" << std::endl;
    }

#ifdef NEKO_FUNCTION_ENABLE_HASH
    // Test 9: Hash functions (if enabled)
    {
        using namespace neko::util::hash;
        auto md5Hash = hash("test", Algorithm::md5);
        assert(md5Hash.length() == 32);
        std::cout << "✓ Hash functions work (enabled)" << std::endl;
    }
#else
    std::cout << "○ Hash functions not enabled" << std::endl;
#endif

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVER
    // Test 10: Archiver detection (if enabled)
    {
        std::cout << "✓ Archiver support compiled (enabled)" << std::endl;
    }
#else
    std::cout << "○ Archiver support not enabled" << std::endl;
#endif
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
    std::cout << "NekoFunction is correctly installed via vcpkg." << std::endl;
    
    return 0;
}
