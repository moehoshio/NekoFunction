/**
 * @file basic_test.cpp
 * @brief Basic functionality tests for NekoFunction library
 * @author moehoshio
 */

#include <neko/function/utilities.hpp>
#include <neko/function/detectFileType.hpp>
#include <neko/function/pattern.hpp>

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVER
#include <neko/function/archiver.hpp>
#endif

#include <iostream>
#include <fstream>
#include <filesystem>

// Simple test framework
class SimpleTest {
private:
    int passed = 0;
    int failed = 0;
    
public:
    void test(bool condition, const std::string& name) {
        if (condition) {
            std::cout << "[PASS] " << name << std::endl;
            passed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            failed++;
        }
    }
    
    int summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total:  " << (passed + failed) << std::endl;
        return failed;
    }
};

int main() {
    SimpleTest test;
    
    std::cout << "NekoFunction Basic Tests" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Test string utilities
    {
        using namespace neko::util::string;
        
        auto quoted = wrapWith("hello", "\"", "\"");
        test.test(quoted == "\"hello\"", "String wrapWith");
        
        auto lower = toLower<std::string>("HELLO");
        test.test(lower == "hello", "String toLower");
        
        auto upper = toUpper<std::string>("world");
        test.test(upper == "WORLD", "String toUpper");
        
        auto ext = getExtensionName("document.txt");
        test.test(ext == "txt", "Get extension");
        
        auto unixPath = convertToUnixPath<std::string>("C:\\test\\file.txt");
        test.test(unixPath == "C:/test/file.txt", "Unix path conversion");
    }
    
    // Test pipe operator
    {
        using namespace neko::ops::pipe;
        using namespace neko::util;

        auto doubleValue = [](int x) { return x * 2; };
        auto result = 5 | doubleValue;
        test.test(result == 10, "Pipe operator");
        
        auto stringResult = std::string("hello") | lambda::plusDoubleQuote;
        test.test(stringResult == "\"hello\"", "Lambda utility");
    }
    
    // Test logic utilities
    {
        using namespace neko::util::logic;
        
        auto str = boolTo<std::string>(true, "yes", "no");
        test.test(str == "yes", "Boolean to string");
        
        bool all = allTrue(true, true, true);
        test.test(all, "All true");
        
        bool any = anyTrue(false, false, true);
        test.test(any, "Any true");
    }
    
    // Test math utilities
    {
        using namespace neko::util::math;
        
        auto total = sum(1, 2, 3, 4, 5);
        test.test(total == 15, "Sum calculation");
        
        auto p = product(2, 3, 4);
        test.test(p == 24, "Product calculation");
    }
    
    // Test Base64
    {
        using namespace neko::util::base64;
        
        auto encoded = base64Encode("Hello");
        test.test(encoded == "SGVsbG8=", "Base64 encode");
        
        auto decoded = base64Decode("SGVsbG8=");
        test.test(decoded == "Hello", "Base64 decode");
    }
    
    // Test UUID
    {
        using namespace neko::util::uuid;
        
        auto uuid4 = uuidV4();
        test.test(uuid4.length() == 36, "UUID v4 length");
        test.test(uuid4[8] == '-' && uuid4[13] == '-', "UUID v4 format");
    }
    
    // Test pattern matching
    {
        using namespace neko::util::pattern;
        
        test.test(containsWildcard("*.txt"), "Wildcard detection");
        test.test(isExtensionPattern(".txt"), "Extension pattern");
        test.test(matchWildcardPattern("file.txt", "*.txt"), "Pattern matching");
    }
    
    // Test validation
    {
        using namespace neko::util::check;
        
        bool validUrl = isUrl("https://example.com");
        test.test(validUrl, "URL validation");
        
        auto res = matchResolution("1920x1080");
        test.test(res.has_value() && res->width == "1920", "Resolution parsing");
    }
    
    // Test hash functions if available
    #ifdef NEKO_ENABLE_HASH_SUPPORT
    {
        using namespace neko::util::hash;
        
        auto md5Hash = hash("test", Algorithm::md5);
        test.test(!md5Hash.empty() && md5Hash.length() == 32, "MD5 hash");
        
        auto sha256Hash = hash("test", Algorithm::sha256);
        test.test(!sha256Hash.empty() && sha256Hash.length() == 64, "SHA256 hash");
    }
    #else
    std::cout << "[INFO] Hash support not available (OpenSSL not found)" << std::endl;
    #endif
    
    // Test archiver if available
    #ifdef NEKO_FUNCTION_ENABLE_ARCHIVER
    {
        std::cout << "[INFO] Archiver support available" << std::endl;
        
        // Create a simple test
        std::filesystem::create_directories("test_dir");
        std::ofstream testFile("test_dir/test.txt");
        testFile << "test content";
        testFile.close();
        
        try {
            using namespace neko::archive;
            
            CreateConfig config;
            config.outputArchivePath = "test.zip";
            config.inputPaths = {"test_dir/"};
            
            zip::create(config);
            test.test(std::filesystem::exists("test.zip"), "Archive creation");
            
            bool isZip = zip::isZipFile("test.zip");
            test.test(isZip, "ZIP detection");
            
            // Cleanup
            std::filesystem::remove("test.zip");
            std::filesystem::remove_all("test_dir");
            
        } catch (const std::exception& e) {
            std::cout << "[INFO] Archive test failed: " << e.what() << std::endl;
        }
    }
    #else
    std::cout << "[INFO] Archiver support not available (minizip-ng not found)" << std::endl;
    #endif
    
    return test.summary();
}