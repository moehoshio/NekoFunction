#pragma once

// Include header for non-module usage
#if !defined(NEKO_FUNCTION_ENABLE_MODULE) || (NEKO_FUNCTION_ENABLE_MODULE == false)

#if defined(NEKO_FUNCTION_ENABLE_HASH)
#include <neko/function/hash.hpp>
#endif // NEKO_FUNCTION_ENABLE_HASH

#include <neko/schema/exception.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>

#endif // NEKO_FUNCTION_ENABLE_MODULE

/**
 * @namespace neko::util::uuid
 * @brief UUID generation and manipulation utilities.
 */
namespace neko::util::uuid {
    /**
     * @brief Generates a version 4 (random) UUID.
     * @return UUID string in standard format
     */
    inline std::string uuidV4() {
        static thread_local std::mt19937 gen{std::random_device{}()};
        static thread_local std::uniform_int_distribution<uint32_t> dis(0, 0xffffffff);

        uint32_t data[4];
        for (auto &d : data)
            d = dis(gen);

        // Set the version (4) and variant (10xx)
        data[1] = (data[1] & 0xFFFF0FFF) | 0x00004000;
        data[2] = (data[2] & 0x3FFFFFFF) | 0x80000000;

        char buf[37];
        std::snprintf(
            buf, sizeof(buf),
            "%08x-%04x-%04x-%04x-%04x%08x",
            data[0],
            (data[1] >> 16) & 0xFFFF, data[1] & 0xFFFF,
            (data[2] >> 16) & 0xFFFF, data[2] & 0xFFFF,
            data[3]);
        return std::string(buf);
    }

    /**
     * @brief Converts a UUID string to bytes.
     * @param uuid UUID string in standard format
     * @return Array of 16 bytes representing the UUID
     */
    inline std::array<uint8_t, 16> uuidStringToBytes(const std::string &uuid) {
        std::array<uint8_t, 16> bytes{};
        int idx = 0;
        for (size_t i = 0; i < uuid.length() && idx < 16; ++i) {
            if (uuid[i] == '-')
                continue;
            uint8_t value = 0;
            if (uuid[i] >= '0' && uuid[i] <= '9')
                value = uuid[i] - '0';
            else if (uuid[i] >= 'a' && uuid[i] <= 'f')
                value = uuid[i] - 'a' + 10;
            else if (uuid[i] >= 'A' && uuid[i] <= 'F')
                value = uuid[i] - 'A' + 10;
            else
                continue;
            value <<= 4;
            ++i;
            if (i < uuid.length() && uuid[i] != '-') {
                if (uuid[i] >= '0' && uuid[i] <= '9')
                    value |= uuid[i] - '0';
                else if (uuid[i] >= 'a' && uuid[i] <= 'f')
                    value |= uuid[i] - 'a' + 10;
                else if (uuid[i] >= 'A' && uuid[i] <= 'F')
                    value |= uuid[i] - 'A' + 10;
            } else {
                --i;
            }
            bytes[idx++] = value;
        }
        return bytes;
    }

    /**
     * @brief Generates a version 3 UUID based on a namespace UUID and a name.
     * @param namespaceUUID Namespace UUID in string format
     * @param name Name to use for UUID generation
     * @return Version 3 UUID string
     * Example Namespace UUID: "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
     *
     * @note This requires using hash values, so OpenSSL support is needed
     * @throws neko::ex::NotImplemented if hash support is not enabled
     */
    inline std::string uuidV3(const std::string &name, const std::string &namespaceUUID = "6ba7b810-9dad-11d1-80b4-00c04fd430c8") {
#if defined(NEKO_FUNCTION_ENABLE_HASH)
        auto ns_bytes = uuidStringToBytes(namespaceUUID);

        std::string to_hash(reinterpret_cast<const char *>(ns_bytes.data()), ns_bytes.size());
        to_hash += name;

        std::string md5hex = hash::digest(to_hash, hash::Algorithm::md5);

        std::array<uint8_t, 16> hash_bytes{};
        for (int i = 0; i < 16; ++i) {
            hash_bytes[i] = static_cast<uint8_t>(std::stoi(md5hex.substr(i * 2, 2), nullptr, 16));
        }

        hash_bytes[6] = (hash_bytes[6] & 0x0F) | 0x30; // version 3
        hash_bytes[8] = (hash_bytes[8] & 0x3F) | 0x80; // RFC4122 variant

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; ++i) {
            oss << std::setw(2) << static_cast<int>(hash_bytes[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9)
                oss << '-';
        }
        return oss.str();
#else
#pragma message("uuid.hpp: uuidV3 requires hash support. Please install OpenSSL and set NEKO_FUNCTION_ENABLE_HASH = ON in CMake.")
        throw ex::NotImplemented("UUID v3 requires hash support. Please compile with NEKO_FUNCTION_ENABLE_HASH=ON and install OpenSSL.");
#endif // NEKO_FUNCTION_ENABLE_HASH
    }

} // namespace neko::util::uuid