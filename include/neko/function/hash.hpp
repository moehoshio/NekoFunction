#pragma once

// Include header for non-module usage
#if !defined(NEKO_FUNCTION_ENABLE_MODULE) || (NEKO_FUNCTION_ENABLE_MODULE == false)

// Include OpenSSL headers for hash functions
#if defined(NEKO_IMPORT_OPENSSL)
#include <openssl/evp.h>

#else
#undef NEKO_FUNCTION_ENABLE_HASH // If no supported hash functions are available, undefine the macro
#endif

#if !defined(NEKO_FUNCTION_ENABLE_HASH)
#error "hash.hpp requires enable hash support. Please install openSSL and set NEKO_FUNCTION_ENABLE_HASH = ON in CMake."
#endif

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#endif // NEKO_FUNCTION_ENABLE_MODULE

/**
 * @namespace neko::util::hash
 * @brief Hash computation utilities.
 */
namespace neko::util::hash {
    /**
     * @enum Algorithm
     * @brief Supported hash algorithms.
     */
    enum class Algorithm {
        none,   ///< No algorithm
        md5,    ///< MD5 algorithm
        sha1,   ///< SHA-1 algorithm
        sha256, ///< SHA-256 algorithm
        sha512  ///< SHA-512 algorithm
    };

    /**
     * @brief Mapping between hash algorithms and their string representations.
     */
    inline std::unordered_map<Algorithm, std::string> hashAlgorithmMap = {
        {Algorithm::md5, "md5"},
        {Algorithm::sha1, "sha1"},
        {Algorithm::sha256, "sha256"},
        {Algorithm::sha512, "sha512"}};

    /**
     * @brief Maps a string to a hash algorithm.
     * @param str String representation of a hash algorithm
     * @return Corresponding hash algorithm enum value
     */
    inline auto mapAlgorithm(const std::string &str) {
        for (auto it : hashAlgorithmMap) {
            if (it.second == str) {
                return it.first;
            }
        }
        return Algorithm::none;
    }

    /**
     * @brief Maps a hash algorithm to its string representation.
     * @param algorithm Hash algorithm enum value
     * @return String representation of the hash algorithm
     */
    inline auto mapAlgorithm(Algorithm algorithm) {
        for (auto it : hashAlgorithmMap) {
            if (it.first == algorithm) {
                return it.second;
            }
        }
        return std::string("unknown");
    }

#if defined(NEKO_IMPORT_OPENSSL)

    /**
     * @brief Computes the hash of a string.
     * @param str String to hash
     * @param algorithm Hash algorithm to use
     * @return Hexadecimal string representation of the hash
     */
    inline std::string digest(const std::string str, Algorithm algorithm = Algorithm::sha256) {
        const unsigned char *unsignedData = reinterpret_cast<const unsigned char *>(str.c_str());
        unsigned char outBuf[EVP_MAX_MD_SIZE];
        unsigned int condLen = 0;

        const EVP_MD *md = nullptr;
        switch (algorithm) {
            case Algorithm::sha1:
                md = EVP_sha1();
                break;
            case Algorithm::sha256:
                md = EVP_sha256();
                break;
            case Algorithm::sha512:
                md = EVP_sha512();
                break;
            case Algorithm::md5:
                md = EVP_md5();
                break;
            default:
                break;
        }

        if (md != nullptr) {
            EVP_MD_CTX *ctx = EVP_MD_CTX_new();
            if (ctx != nullptr) {
                EVP_DigestInit_ex(ctx, md, nullptr);
                EVP_DigestUpdate(ctx, unsignedData, str.size());
                EVP_DigestFinal_ex(ctx, outBuf, &condLen);
                EVP_MD_CTX_free(ctx);
            }
        }

        std::stringstream ssRes;
        for (unsigned int i = 0; i < condLen; ++i) {
            ssRes << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(outBuf[i]);
        }
        return ssRes.str();
    }

    /**
     * @brief Computes the hash of a file.
     * @param name Path to the file
     * @param algorithm Hash algorithm to use
     * @return Hexadecimal string representation of the hash
     */
    inline std::string digestFile(const std::string &name, Algorithm algorithm = Algorithm::sha256) {
        std::ifstream file(name, std::ios::binary);
        std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return digest(raw, algorithm);
    }
#endif // NEKO_IMPORT_OPENSSL

} // namespace neko::util::hash