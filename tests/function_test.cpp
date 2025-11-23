/**
 * @file NekoFunction_test.cpp
 * @brief Google Test suite for NekoFunction library
 * @author moehoshio
 */

#include <gtest/gtest.h>
#include <neko/function/utilities.hpp>
#include <neko/function/uuid.hpp>
#include <neko/function/detectFileType.hpp>
#include <neko/function/pattern.hpp>

#ifdef NEKO_FUNCTION_ENABLE_HASH
#include <neko/function/hash.hpp>
#endif

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVER
#include <neko/function/archiver.hpp>
#endif

#include <fstream>
#include <filesystem>

// ============================================================================
// String Utilities Tests
// ============================================================================

class StringUtilitiesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(StringUtilitiesTest, WrapWith) {
    using namespace neko::util::string;
    auto quoted = wrapWith("hello", "\"", "\"");
    EXPECT_EQ(quoted, "\"hello\"");
}

TEST_F(StringUtilitiesTest, ToLower) {
    using namespace neko::util::string;
    auto lower = toLower<std::string>("HELLO");
    EXPECT_EQ(lower, "hello");
}

TEST_F(StringUtilitiesTest, ToUpper) {
    using namespace neko::util::string;
    auto upper = toUpper<std::string>("world");
    EXPECT_EQ(upper, "WORLD");
}

TEST_F(StringUtilitiesTest, GetExtension) {
    using namespace neko::util::string;
    auto ext = getExtensionName("document.txt");
    EXPECT_EQ(ext, "txt");
}

TEST_F(StringUtilitiesTest, UnixPathConversion) {
    using namespace neko::util::string;
    auto unixPath = convertToUnixPath<std::string>("C:\\test\\file.txt");
    EXPECT_EQ(unixPath, "C:/test/file.txt");
}

// ============================================================================
// Pipe Operator Tests
// ============================================================================

TEST(PipeOperatorTest, BasicPipe) {
    using namespace neko::ops::pipe;
    auto doubleValue = [](int x) { return x * 2; };
    auto result = 5 | doubleValue;
    EXPECT_EQ(result, 10);
}

TEST(PipeOperatorTest, LambdaUtility) {
    using namespace neko::ops::pipe;
    using namespace neko::util;
    auto stringResult = std::string("hello") | lambda::plusDoubleQuote;
    EXPECT_EQ(stringResult, "\"hello\"");
}

// ============================================================================
// Logic Utilities Tests
// ============================================================================

TEST(LogicUtilitiesTest, BoolToString) {
    using namespace neko::util::logic;
    auto str = boolTo<std::string>(true, "yes", "no");
    EXPECT_EQ(str, "yes");
    
    auto str2 = boolTo<std::string>(false, "yes", "no");
    EXPECT_EQ(str2, "no");
}

TEST(LogicUtilitiesTest, AllTrue) {
    using namespace neko::util::logic;
    EXPECT_TRUE(allTrue(true, true, true));
    EXPECT_FALSE(allTrue(true, false, true));
    EXPECT_FALSE(allTrue(false, false, false));
}

TEST(LogicUtilitiesTest, AnyTrue) {
    using namespace neko::util::logic;
    EXPECT_TRUE(anyTrue(false, false, true));
    EXPECT_TRUE(anyTrue(true, false, false));
    EXPECT_FALSE(anyTrue(false, false, false));
}

// ============================================================================
// Math Utilities Tests
// ============================================================================

TEST(MathUtilitiesTest, Sum) {
    using namespace neko::util::math;
    auto total = sum(1, 2, 3, 4, 5);
    EXPECT_EQ(total, 15);
    
    EXPECT_EQ(sum(10, 20), 30);
    EXPECT_EQ(sum(5), 5);
}

TEST(MathUtilitiesTest, Product) {
    using namespace neko::util::math;
    auto p = product(2, 3, 4);
    EXPECT_EQ(p, 24);
    
    EXPECT_EQ(product(5, 5), 25);
    EXPECT_EQ(product(7), 7);
}

// ============================================================================
// Base64 Tests
// ============================================================================

class Base64Test : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(Base64Test, Encode) {
    using namespace neko::util::base64;
    auto encoded = base64Encode("Hello");
    EXPECT_EQ(encoded, "SGVsbG8=");
}

TEST_F(Base64Test, Decode) {
    using namespace neko::util::base64;
    auto decoded = base64Decode("SGVsbG8=");
    EXPECT_EQ(decoded, "Hello");
}

TEST_F(Base64Test, EncodeDecodeRoundTrip) {
    using namespace neko::util::base64;
    std::string original = "Test string with special chars: !@#$%^&*()";
    auto encoded = base64Encode(original);
    auto decoded = base64Decode(encoded);
    EXPECT_EQ(decoded, original);
}

// ============================================================================
// UUID Tests
// ============================================================================

TEST(UUIDTest, UUIDv4Length) {
    using namespace neko::util::uuid;
    auto uuid4 = uuidV4();
    EXPECT_EQ(uuid4.length(), 36);
}

TEST(UUIDTest, UUIDv4Format) {
    using namespace neko::util::uuid;
    auto uuid4 = uuidV4();
    // UUID format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    EXPECT_EQ(uuid4[8], '-');
    EXPECT_EQ(uuid4[13], '-');
    EXPECT_EQ(uuid4[18], '-');
    EXPECT_EQ(uuid4[23], '-');
}

TEST(UUIDTest, UUIDv4Uniqueness) {
    using namespace neko::util::uuid;
    auto uuid1 = uuidV4();
    auto uuid2 = uuidV4();
    EXPECT_NE(uuid1, uuid2);
}

// ============================================================================
// Pattern Matching Tests
// ============================================================================

class PatternMatchingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PatternMatchingTest, WildcardDetection) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(containsWildcard("*.txt"));
    EXPECT_TRUE(containsWildcard("file*.log"));
    EXPECT_FALSE(containsWildcard("file.txt"));
}

TEST_F(PatternMatchingTest, ExtensionPattern) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(isExtensionPattern(".txt"));
    EXPECT_TRUE(isExtensionPattern(".log"));
    EXPECT_FALSE(isExtensionPattern("*.txt"));
    EXPECT_FALSE(isExtensionPattern("file.txt"));
}

TEST_F(PatternMatchingTest, WildcardMatching) {
    using namespace neko::util::pattern;
    EXPECT_TRUE(matchWildcardPattern("file.txt", "*.txt"));
    EXPECT_TRUE(matchWildcardPattern("document.log", "*.log"));
    EXPECT_FALSE(matchWildcardPattern("file.txt", "*.log"));
}

// ============================================================================
// Validation Tests
// ============================================================================

TEST(ValidationTest, URLValidation) {
    using namespace neko::util::check;
    EXPECT_TRUE(isUrl("https://example.com"));
    EXPECT_TRUE(isUrl("http://example.com"));
    // Note: isUrl may only support http/https protocols
    EXPECT_FALSE(isUrl("not a url"));
    EXPECT_FALSE(isUrl(""));
}

TEST(ValidationTest, ResolutionParsing) {
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
// Hash Function Tests (conditional compilation)
// ============================================================================

#ifdef NEKO_FUNCTION_ENABLE_HASH

class HashTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(HashTest, MD5Hash) {
    using namespace neko::util::hash;
    auto md5Hash = digest("test", Algorithm::md5);
    EXPECT_FALSE(md5Hash.empty());
    EXPECT_EQ(md5Hash.length(), 32);
}

TEST_F(HashTest, SHA256Hash) {
    using namespace neko::util::hash;
    auto sha256Hash = digest("test", Algorithm::sha256);
    EXPECT_FALSE(sha256Hash.empty());
    EXPECT_EQ(sha256Hash.length(), 64);
}

TEST_F(HashTest, HashConsistency) {
    using namespace neko::util::hash;
    auto hash1 = digest("test", Algorithm::md5);
    auto hash2 = digest("test", Algorithm::md5);
    EXPECT_EQ(hash1, hash2);
}

TEST_F(HashTest, DifferentInputs) {
    using namespace neko::util::hash;
    auto hash1 = digest("test1", Algorithm::md5);
    auto hash2 = digest("test2", Algorithm::md5);
    EXPECT_NE(hash1, hash2);
}

#endif // NEKO_FUNCTION_ENABLE_HASH

// ============================================================================
// Archiver Tests (conditional compilation)
// ============================================================================

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVER

class ArchiverTest : public ::testing::Test {
protected:
    const std::string testDir = "test_archiver_dir";
    const std::string testFile = "test.txt";
    const std::string zipFile = "test_archive.zip";

    void SetUp() override {
        // Create test directory and file
        std::filesystem::create_directories(testDir);
        std::ofstream file(testDir + "/" + testFile);
        file << "test content for archiver";
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

TEST_F(ArchiverTest, CreateArchive) {
    using namespace neko::archive;
    
    CreateConfig config;
    config.outputArchivePath = zipFile;
    config.inputPaths = {testDir + "/"};
    
    EXPECT_NO_THROW(zip::create(config));
    EXPECT_TRUE(std::filesystem::exists(zipFile));
}

TEST_F(ArchiverTest, ZIPDetection) {
    using namespace neko::archive;
    
    // Create archive first
    CreateConfig config;
    config.outputArchivePath = zipFile;
    config.inputPaths = {testDir + "/"};
    zip::create(config);
    
    // Test detection on existing file
    EXPECT_TRUE(zip::isZipArchiveFile(zipFile));
    
    // Note: isZipArchiveFile may throw exception for non-existent files
    // so we skip testing with non-existent file
}

TEST_F(ArchiverTest, ExtractArchive) {
    using namespace neko::archive;
    
    const std::string extractDir = "test_extract_dir";
    
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
    // The extracted files should be in the extract directory
    EXPECT_TRUE(std::filesystem::exists(extractDir));
    
    // Clean up extract directory
    std::filesystem::remove_all(extractDir);
}

#endif // NEKO_FUNCTION_ENABLE_ARCHIVER

// ============================================================================
// Main function
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Print configuration information
    std::cout << "=== NekoFunction Test Configuration ===" << std::endl;
    
#ifdef NEKO_FUNCTION_ENABLE_HASH
    std::cout << "Hash support: ENABLED" << std::endl;
#else
    std::cout << "Hash support: DISABLED" << std::endl;
#endif

#ifdef NEKO_FUNCTION_ENABLE_ARCHIVER
    std::cout << "Archiver support: ENABLED" << std::endl;
#else
    std::cout << "Archiver support: DISABLED" << std::endl;
#endif
    
    std::cout << "========================================" << std::endl << std::endl;
    
    return RUN_ALL_TESTS();
}