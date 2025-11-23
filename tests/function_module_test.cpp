/**
 * @file function_module_test.cpp
 * @brief Google Test suite for NekoFunction C++20 Module
 * @author moehoshio
 * @details Tests the NekoFunction library using C++20 module imports
 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

import neko.function;

// ============================================================================
// String Utilities Tests (Module)
// ============================================================================

class StringUtilitiesModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(StringUtilitiesModuleTest, WrapWith) {
    using namespace neko::util::string;
    auto quoted = wrapWith("hello", "\"", "\"");
    EXPECT_EQ(quoted, "\"hello\"");
}

TEST_F(StringUtilitiesModuleTest, ToLower) {
    using namespace neko::util::string;
    auto lower = toLower<std::string>("HELLO");
    EXPECT_EQ(lower, "hello");
}

TEST_F(StringUtilitiesModuleTest, ToUpper) {
    using namespace neko::util::string;
    auto upper = toUpper<std::string>("world");
    EXPECT_EQ(upper, "WORLD");
}

TEST_F(StringUtilitiesModuleTest, GetExtension) {
    using namespace neko::util::string;
    auto ext = getExtensionName("document.txt");
    EXPECT_EQ(ext, "txt");
}

TEST_F(StringUtilitiesModuleTest, UnixPathConversion) {
    using namespace neko::util::string;
    auto unixPath = convertToUnixPath<std::string>("C:\\test\\file.txt");
    EXPECT_EQ(unixPath, "C:/test/file.txt");
}

// ============================================================================
// Pipe Operator Tests (Module)
// ============================================================================

TEST(PipeOperatorModuleTest, BasicPipe) {
    using namespace neko::ops::pipe;
    auto doubleValue = [](int x) { return x * 2; };
    auto result = 5 | doubleValue;
    EXPECT_EQ(result, 10);
}

TEST(PipeOperatorModuleTest, LambdaUtility) {
    using namespace neko::ops::pipe;
    using namespace neko::util;
    auto stringResult = std::string("hello") | lambda::plusDoubleQuote;
    EXPECT_EQ(stringResult, "\"hello\"");
}

// ============================================================================
// Logic Utilities Tests (Module)
// ============================================================================

TEST(LogicUtilitiesModuleTest, BoolToString) {
    using namespace neko::util::logic;
    auto str = boolTo<std::string>(true, "yes", "no");
    EXPECT_EQ(str, "yes");
    
    auto str2 = boolTo<std::string>(false, "yes", "no");
    EXPECT_EQ(str2, "no");
}

TEST(LogicUtilitiesModuleTest, AllTrue) {
    using namespace neko::util::logic;
    EXPECT_TRUE(allTrue(true, true, true));
    EXPECT_FALSE(allTrue(true, false, true));
    EXPECT_FALSE(allTrue(false, false, false));
}

TEST(LogicUtilitiesModuleTest, AnyTrue) {
    using namespace neko::util::logic;
    EXPECT_TRUE(anyTrue(false, false, true));
    EXPECT_TRUE(anyTrue(true, false, false));
    EXPECT_FALSE(anyTrue(false, false, false));
}

// ============================================================================
// Math Utilities Tests (Module)
// ============================================================================

TEST(MathUtilitiesModuleTest, Sum) {
    using namespace neko::util::math;
    auto total = sum(1, 2, 3, 4, 5);
    EXPECT_EQ(total, 15);
    
    EXPECT_EQ(sum(10, 20), 30);
    EXPECT_EQ(sum(5), 5);
}

TEST(MathUtilitiesModuleTest, Product) {
    using namespace neko::util::math;
    auto p = product(2, 3, 4);
    EXPECT_EQ(p, 24);
    
    EXPECT_EQ(product(5, 5), 25);
    EXPECT_EQ(product(7), 7);
}

// ============================================================================
// Base64 Tests (Module)
// ============================================================================

class Base64ModuleTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(Base64ModuleTest, Encode) {
    using namespace neko::util::base64;
    auto encoded = base64Encode("Hello");
    EXPECT_EQ(encoded, "SGVsbG8=");
}

TEST_F(Base64ModuleTest, Decode) {
    using namespace neko::util::base64;
    auto decoded = base64Decode("SGVsbG8=");
    EXPECT_EQ(decoded, "Hello");
}

TEST_F(Base64ModuleTest, EncodeDecodeRoundTrip) {
    using namespace neko::util::base64;
    std::string original = "Test string with special chars: !@#$%^&*()";
    auto encoded = base64Encode(original);
    auto decoded = base64Decode(encoded);
    EXPECT_EQ(decoded, original);
}

// ============================================================================
// UUID Tests (Module)
// ============================================================================

TEST(UUIDModuleTest, UUIDv4Length) {
    using namespace neko::util::uuid;
    auto uuid4 = uuidV4();
    EXPECT_EQ(uuid4.length(), 36);
}

TEST(UUIDModuleTest, UUIDv4Format) {
    using namespace neko::util::uuid;
    auto uuid4 = uuidV4();
    // UUID format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    EXPECT_EQ(uuid4[8], '-');
    EXPECT_EQ(uuid4[13], '-');
    EXPECT_EQ(uuid4[18], '-');
    EXPECT_EQ(uuid4[23], '-');
}

TEST(UUIDModuleTest, UUIDv4Uniqueness) {
    using namespace neko::util::uuid;
    auto uuid1 = uuidV4();
    auto uuid2 = uuidV4();
    EXPECT_NE(uuid1, uuid2);
}

// ============================================================================
// Pattern Matching Tests (Module)
// ============================================================================

class PatternMatchingModuleTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PatternMatchingModuleTest, WildcardDetection) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(containsWildcard("*.txt"));
    EXPECT_TRUE(containsWildcard("file*.log"));
    EXPECT_FALSE(containsWildcard("file.txt"));
}

TEST_F(PatternMatchingModuleTest, ExtensionPattern) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(isExtensionPattern(".txt"));
    EXPECT_TRUE(isExtensionPattern(".log"));
    EXPECT_FALSE(isExtensionPattern("*.txt"));
    EXPECT_FALSE(isExtensionPattern("file.txt"));
}

TEST_F(PatternMatchingModuleTest, WildcardMatching) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(matchWildcardPattern("file.txt", "*.txt"));
    EXPECT_TRUE(matchWildcardPattern("document.log", "*.log"));
    EXPECT_FALSE(matchWildcardPattern("file.txt", "*.log"));
}

// ============================================================================
// Validation Tests (Module)
// ============================================================================

TEST(ValidationModuleTest, URLValidation) {
    using namespace neko::util::check;
    EXPECT_TRUE(isUrl("https://example.com"));
    EXPECT_TRUE(isUrl("http://example.com"));
    EXPECT_FALSE(isUrl("not a url"));
    EXPECT_FALSE(isUrl(""));
}

TEST(ValidationModuleTest, ResolutionParsing) {
    using namespace neko::util::check;
    auto res = matchResolution("1920x1080");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->width, "1920");
    EXPECT_EQ(res->height, "1080");
    
    auto res2 = matchResolution("800x600");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->width, "800");
    EXPECT_EQ(res2->height, "600");
    
    auto invalid = matchResolution("invalid");
    EXPECT_FALSE(invalid.has_value());
}

// ============================================================================
// Hash Function Tests (Module, conditional compilation)
// ============================================================================

#ifdef NEKO_FUNCTION_ENABLE_HASH

class HashModuleTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HashModuleTest, MD5Hash) {
    using namespace neko::util::hash;
    auto md5Hash = digest("test", Algorithm::md5);
    EXPECT_FALSE(md5Hash.empty());
    EXPECT_EQ(md5Hash.length(), 32);
}

TEST_F(HashModuleTest, SHA256Hash) {
    using namespace neko::util::hash;
    auto sha256Hash = digest("test", Algorithm::sha256);
    EXPECT_FALSE(sha256Hash.empty());
    EXPECT_EQ(sha256Hash.length(), 64);
}

TEST_F(HashModuleTest, HashConsistency) {
    using namespace neko::util::hash;
    auto hash1 = digest("test", Algorithm::md5);
    auto hash2 = digest("test", Algorithm::md5);
    EXPECT_EQ(hash1, hash2);
}

TEST_F(HashModuleTest, DifferentInputs) {
    using namespace neko::util::hash;
    auto hash1 = digest("test1", Algorithm::md5);
    auto hash2 = digest("test2", Algorithm::md5);
    EXPECT_NE(hash1, hash2);
}

#endif // NEKO_FUNCTION_ENABLE_HASH

// ============================================================================
// Archiver Tests (Module, conditional compilation)
// ============================================================================

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVE

class ArchiverModuleTest : public ::testing::Test {
protected:
    const std::string testDir = "test_archiver_module_dir";
    const std::string testFile = "test_module.txt";
    const std::string zipFile = "test_archive_module.zip";

    void SetUp() override {
        // Create test directory and file
        std::filesystem::create_directories(testDir);
        std::ofstream file(testDir + "/" + testFile);
        file << "test content for archiver module";
        file.close();
    }

    void TearDown() override {
        // Cleanup
        if (std::filesystem::exists(zipFile)) {
            std::filesystem::remove(zipFile);
        }
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
};

TEST_F(ArchiverModuleTest, CreateArchive) {
    using namespace neko::archive;
    
    CreateConfig config;
    config.outputArchivePath = zipFile;
    config.inputPaths = {testDir + "/"};
    
    EXPECT_NO_THROW(zip::create(config));
    EXPECT_TRUE(std::filesystem::exists(zipFile));
}

TEST_F(ArchiverModuleTest, ZIPDetection) {
    using namespace neko::archive;
    
    // Create archive first
    CreateConfig config;
    config.outputArchivePath = zipFile;
    config.inputPaths = {testDir + "/"};
    zip::create(config);
    
    // Test detection on existing file
    EXPECT_TRUE(zip::isZipArchiveFile(zipFile));
}

TEST_F(ArchiverModuleTest, ExtractArchive) {
    using namespace neko::archive;
    
    const std::string extractDir = "test_extract_module_dir";
    
    // Create archive
    CreateConfig createConfig;
    createConfig.outputArchivePath = zipFile;
    createConfig.inputPaths = {testDir + "/"};
    zip::create(createConfig);
    
    // Ensure extract directory doesn't exist
    if (std::filesystem::exists(extractDir)) {
        std::filesystem::remove_all(extractDir);
    }
    std::filesystem::create_directories(extractDir);
    
    // Extract archive
    ExtractConfig extractConfig;
    extractConfig.inputArchivePath = zipFile;
    extractConfig.destDir = extractDir;
    
    EXPECT_NO_THROW(zip::extract(extractConfig));
    
    // Check if extraction was successful
    EXPECT_TRUE(std::filesystem::exists(extractDir));
    
    // Clean up extract directory
    std::filesystem::remove_all(extractDir);
}

#endif // NEKO_FUNCTION_ENABLE_ARCHIVE

// ============================================================================
// Module-Specific Tests
// ============================================================================

TEST(NekoFunctionModuleTest, ModuleImportWorks) {
    // This test simply verifies that the module import is working
    // by using a basic function from the module
    using namespace neko::util::string;
    auto result = wrapWith("module", "[", "]");
    EXPECT_EQ(result, "[module]");
}

TEST(NekoFunctionModuleTest, NamespaceAccessibility) {
    // Verify that all expected namespaces are accessible
    using namespace neko::util;
    
    // String utilities
    auto str = string::toLower<std::string>("TEST");
    EXPECT_EQ(str, "test");
    
    // Math utilities
    auto sum = math::sum(1, 2, 3);
    EXPECT_EQ(sum, 6);
    
    // Logic utilities
    auto result = logic::boolTo<std::string>(true, "T", "F");
    EXPECT_EQ(result, "T");
}

// ============================================================================
// Main function
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Print configuration information
    std::cout << "=== NekoFunction Module Test Configuration ===" << std::endl;
    std::cout << "Testing with C++20 Modules enabled" << std::endl;
    
#ifdef NEKO_FUNCTION_ENABLE_HASH
    std::cout << "Hash support: ENABLED" << std::endl;
#else
    std::cout << "Hash support: DISABLED" << std::endl;
#endif

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVE
    std::cout << "Archive support: ENABLED" << std::endl;
#else
    std::cout << "Archive support: DISABLED" << std::endl;
#endif
    
    std::cout << "================================================" << std::endl << std::endl;
    
    return RUN_ALL_TESTS();
}
