/**
 * @file utilities.hpp
 * @brief Utilities for time handling, base64 encoding/decoding, path handling, and string manipulation.
 * @author moehoshio
 * @copyright Copyright (c) 2025 Hoshi
 * @license MIT OR Apache-2.0
 * @link https://github.com/moehoshio/NekoFunction
 *
 * This header provides a collection of utilities:
 * - Functional programming operators
 * - Path manipulation and normalization
 * - Base64 encoding/decoding
 * - String utilities
 * - Random value generators
 * - File extension matching
 */
#pragma once

// Include header for non-module usage
#if !defined(NEKO_FUNCTION_ENABLE_MODULE) || (NEKO_FUNCTION_ENABLE_MODULE == false)

// C++ Standard Library
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

#endif // NEKO_FUNCTION_ENABLE_MODULE
/**
 * @namespace neko::ops
 * @brief Operator overloads for functional programming style operations.
 */
namespace neko::ops {

    namespace pipe {
        /**
         * @brief Pipe operator for functional composition.
         * @param val Value to be passed to the function
         * @param func Function to be applied to the value
         * @return Result of applying the function to the value
         * @example auto result = 5 | [](int x) { return x * 2; } ; // result will be 10
         */
        constexpr decltype(auto) operator|(auto &&val, auto &&func) noexcept(noexcept(func(std::forward<decltype(val)>(val)))) {
            static_assert(requires { func(std::forward<decltype(val)>(val)); }, "val must be a valid argument for func");
            return func(std::forward<decltype(val)>(val));
        }
    } // namespace pipe

    template <typename T>
    concept OptionalLike = requires(T t) {
        { bool(t) };
        { *t };
    };

    namespace fallback {
        /**
         * @brief Fallback operator for chaining functions with optional results.
         * @param f First function to be applied
         * @param g Second function to be applied if the first returns no result
         * @return A new function that applies both functions in sequence
         * @note The return type of the first function (F1) must be convertible to bool and support dereferencing (*).
         * @example
         * auto try_parse_a = [](std::string s) -> std::optional<std::string> { ... };
         * auto try_parse_b =  [](std::string s) -> std::string { log error; return "default"; };
         * auto parse = try_parse_a || try_parse_b;
         * auto result = parse("some_str");
         */
        constexpr auto operator||(auto &&f, auto &&g) noexcept {
            return [f = std::forward<decltype(f)>(f), g = std::forward<decltype(g)>(g)](auto &&x) -> decltype(auto) {
                auto r = f(x);
                using RType = decltype(r);
                static_assert(OptionalLike<RType>, "Return type of f(x) must be optional-like.");
                if (r)
                    return *r;
                return g(std::forward<decltype(x)>(x));
            };
        }
    } // namespace fallback

    namespace logic {
        /**
         * @brief If the optional has a value, continue chaining; otherwise return an empty value
         * @param opt Optional value to be passed to the function.
         * @param f Function to be applied to the optional value
         * @return Result of applying the function to the optional value
         * @example
         * std::optional<int> parse(std::string s) { ... }
         * std::optional<double> to_double(int x) { ... }
         * std::optional<double> result = parse("42") >>= to_double;
         */
        template <OptionalLike Opt, typename F>
        constexpr auto operator>>=(Opt &&opt, F &&f) {
            if (opt)
                return f(*opt);
            using OptType = std::decay_t<decltype(opt)>;
            return OptType{};
        }
    } // namespace logic

    /**
     * @brief Pipe operator overload for ranges.
     * @namespace neko::ops::ranges
     * @note This conflicts with the syntax of std::ranges::views.
     */
    namespace ranges {
        /**
         * @brief Pipe operator overload for initializer lists.
         * @tparam T Type of elements in the initializer list
         * @tparam F Type of the function
         * @param val Initializer list to be passed to the function
         * @param func Function to be applied to the initializer list
         * @return Result of applying the function to the initializer list
         * @example auto result = {1, 2, 3} | [](const std::initializer_list<int> &list) {
         *     return std::accumulate(list.begin(), list.end(), 0);
         * };
         */
        template <typename Range, typename F>
        constexpr decltype(auto) operator|(Range &&range, F &&func)
            requires requires { std::begin(range); std::end(range); }
        {
            return func(std::forward<Range>(range));
        }
    } // namespace ranges

    /**
     * @brief Pipe operator overload for applying a function to each element in an initializer list.
     * @namespace neko::ops::foreach
     * @note This conflicts with the syntax of std::ranges::views.
     */
    namespace foreach {
        /**
         * @brief Pipe operator overload for applying a function to each element in an initializer list.
         * @tparam T Type of elements in the initializer list
         * @tparam F Type of the function
         * @param val Initializer list whose elements will be passed to the function
         * @param func Function to be applied to each element
         * @return void
         * @example {1, 2, 3} | [](int x) { std::cout << x << std::endl; };
         */
        template <typename Range, typename F>
            requires requires(Range &&r) { std::begin(r); std::end(r); } && requires(F &&f, decltype(*std::begin(std::declval<Range &>())) v) {
                std::forward<F>(f)(v);
            }
        constexpr void operator|(Range &&range, F &&func) {
            for (auto &&elem : range) {
                std::forward<F>(func)(elem);
            }
        }
    } // namespace foreach

} // namespace neko::ops

/**
 * @namespace neko::util
 * @brief Utility functions
 */
namespace neko::util {

    /**
     * @namespace neko::util::string
     * @brief String manipulation utilities.
     */
    namespace string {

        /**
         * @brief Adds specified prefix and suffix around a string.
         * @tparam T String type
         * @param str String to wrap
         * @param prefix Content to add before str
         * @param suffix Content to add after str
         * @return String with prefix and suffix added
         */
        template <typename T1, typename T2 = std::string, typename T3 = std::string>
        constexpr auto wrapWith(const T1 &str, const T2 &prefix = "\"", const T3 &suffix = "\"") {
            using StrType = std::conditional_t<
                std::is_convertible_v<T1, std::string> && !std::is_same_v<std::decay_t<T1>, std::string>,
                std::string,
                std::decay_t<T1>>;
            return static_cast<StrType>(prefix) + static_cast<StrType>(str) + static_cast<StrType>(suffix);
        }

        /**
         * @brief Converts a string to lowercase.
         * @tparam StringType String type (e.g., std::string, std::wstring)
         * @param str String to convert
         * @return Lowercase version of the string
         */
        template <typename StringType = std::string>
        constexpr StringType toLower(const StringType &str) {
            StringType lowerStr = str;
            std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                           [](auto ch) { return std::tolower(ch); });
            return lowerStr;
        }

        /**
         * @brief Converts a string to uppercase.
         * @tparam StringType String type (e.g., std::string, std::wstring)
         * @param str String to convert
         * @return Uppercase version of the string
         */
        template <typename StringType = std::string>
        constexpr StringType toUpper(const StringType &str) {
            StringType upperStr = str;
            std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(),
                           [](auto ch) { return std::toupper(ch); });
            return upperStr;
        }

        /**
         * @brief Gets the file extension from a filename.
         * @param filename Name of the file
         * @param caseSensitive Whether to convert the result to all lowercase
         * @return File extension (e.g. "txt")
         */
        inline std::string getExtensionName(const std::string &filename, bool caseSensitive = false) {
            auto pos = filename.find_last_of('.');
            if (pos == std::string::npos)
                return "";
            std::string ext = filename.substr(pos + 1);
            if (!caseSensitive) {
                ext = toLower(ext);
            }
            return ext;
        }

        /**
         * @brief Checks if a file has a specific extension.
         * @param name Name to check. e.g. "/path/to/file.txt"
         * @param targetExtension Extension to match. e.g. "txt"
         * @param caseSensitive Whether to perform case-sensitive matching
         * @return true if the file has the specified extension, false otherwise
         */
        inline bool matchExtensionName(std::string_view name, std::string_view targetExtension, bool caseSensitive = false) {
            std::string ext = getExtensionName(std::string(name), caseSensitive);
            std::string target(targetExtension);
            if (!caseSensitive) {
                ext = toLower(ext);
                target = toLower(target);
            }
            return ext == target;
        }

        /**
         * @brief Checks if a file has any of the specified extensions.
         * @param name Name to check. e.g. "/path/to/file.txt"
         * @param targetExtensions Vector of extensions to match. e.g. {"txt", "md", "docx"}
         * @return true if the file has any of the specified extensions, false otherwise
         */
        inline bool matchExtensionNames(const std::string &name, const std::vector<std::string> &targetExtensions, bool caseSensitive = false) {
            for (const auto &it : targetExtensions) {
                if (matchExtensionName(name, it, caseSensitive)) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Normalizes path separators to forward slashes.
         * @tparam T String type
         * @param inPath Path to normalize
         * @return Normalized path with forward slashes
         */
        template <typename T = std::string>
        constexpr T convertToUnixPath(const T &inPath) {
            T res = inPath;
            std::replace(res.begin(), res.end(), '\\', '/');
            return res;
        }

    } // namespace string

    namespace memory {

        /**
         * @brief Creates a std::array from the given arguments.
         * @tparam Args Types of the arguments
         * @param args Arguments to be included in the array
         * @return std::array containing the provided arguments
         */
        constexpr auto makeArray(auto &&...args) {
            return std::array{std::forward<decltype(args)>(args)...};
        }

        /**
         * @brief Creates a shallow copy of a value.
         */
        template <typename T>
        constexpr std::decay_t<T> copy(const T &d) {
            return T(d);
        }

        /**
         * @brief Creates a deep copy of a pointer.
         * @param ptr Pointer to copy
         * @return New pointer with copied value, or nullptr if ptr is null
         */
        template <typename T>
        constexpr std::remove_cv_t<T> *copy(const T *ptr) {
            static_assert(!std::is_void_v<T>, "Cannot deep copy void pointer");
            static_assert(std::is_copy_constructible_v<T>, "deepCopy requires copy-constructible type");
            if (ptr) {
                return new std::remove_cv_t<T>(*ptr);
            }
            return static_cast<std::remove_cv_t<T> *>(nullptr);
        }

    } // namespace memory

    /**
     * @namespace neko::util::lambda
     * @brief Lambda utilities
     */
    inline namespace lambda {
        /**
         * @brief Lambda for adding double quotes to a value.
         */
        constexpr auto plusDoubleQuote = [](auto &&val) { return string::wrapWith(val, "\"", "\""); };

        /**
         * @brief Lambda for adding single quotes to a value.
         */
        constexpr auto plusSingleQuote = [](auto &&val) { return string::wrapWith(val, "'", "'"); };

        /**
         * @brief Lambda for normalizing path separators.
         */
        constexpr auto unifiedPath = [](auto &&val) { return string::convertToUnixPath(val); };

        /**
         * @brief Lambda for moving a value.
         */
        constexpr auto move = [](auto &&val) -> auto && { return std::move(val); };

        /**
         * @brief Lambda for creating a shared pointer.
         */
        constexpr auto makeShared = [](auto &&val) -> decltype(auto) { return std::make_shared<std::remove_reference_t<decltype(val)>>(std::forward<decltype(val)>(val)); };

        /**
         * @brief Creates shallow copy of value semantics
         * @param d Value to copy
         * @return Copy of the value
         */
        constexpr auto copy = [](const auto &d) { return memory::copy(d); };

        /**
         * @brief Creates a deep copy of a pointer.
         * @param ptr Pointer to copy
         * @return New pointer with copied value, or nullptr if ptr is null
         */
        constexpr auto deepCopy = [](const auto *ptr) -> std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>> * { return memory::copy(ptr); };
    } // namespace lambda

    inline namespace logic {
        /**
         * @brief Converts a boolean to a string or other type.
         * @tparam T Return type
         * @param v Boolean value to convert
         * @param rTrue Value to return if v is true
         * @param rFalse Value to return if v is false
         * @return rTrue if v is true, rFalse otherwise
         */
        template <typename T = std::string>
        constexpr T boolTo(bool v, const T &rTrue = T("true"), const T &rFalse = T("false")) {
            return ((v) ? rTrue : rFalse);
        };
        /**
         * @brief Checks if all arguments are true.
         * @param args Boolean values to check
         * @return true if all arguments are true, false otherwise
         */
        constexpr bool allTrue(auto &&...args) {
            return (true && ... && args);
        };

        /**
         * @brief Checks if any argument is true.
         * @param args Boolean values to check
         * @return true if any argument is true, false otherwise
         */
        constexpr bool anyTrue(auto &&...args) {
            return (false || ... || args);
        };
    } // namespace logic

    inline namespace math {

        /**
         * @brief Computes the sum of all arguments.
         * @param args Values to sum
         * @return Sum of all arguments
         */
        constexpr auto sum(auto &&...args) {
            static_assert(sizeof...(args) > 0, "sum requires at least one argument");
            return (args + ...);
        };

        /**
         * @brief Computes the product of all arguments.
         * @param args Values to multiply
         * @return Product of all arguments
         */
        constexpr auto product(auto &&...args) {
            static_assert(sizeof...(args) > 0, "product requires at least one argument");
            return (args * ...);
        };

    } // namespace math

    /**
     * @namespace neko::util::time
     * @brief Time-related utilities.
     */
    namespace time {

        /**
         * @brief Converts std::time_t to local std::tm.
         * @param t Utc timestamp
         * @return Local std::tm structure
         */
        inline std::tm toLocalTm(std::time_t t) {
            std::tm tmResult = {};
#if defined(_WIN32) || defined(_WIN64)
            localtime_s(&tmResult, &t);
#else
            localtime_r(&t, &tmResult);
#endif
            return tmResult;
        }

        /**
         * @brief Converts std::time_t to UTC std::tm.
         * @param t Utc timestamp
         * @return UTC std::tm structure
         */
        inline std::tm toUtcTm(std::time_t t) {
            std::tm tmResult = {};
#if defined(_WIN32) || defined(_WIN64)
            gmtime_s(&tmResult, &t);
#else
            gmtime_r(&t, &tmResult);
#endif
            return tmResult;
        }

        /**
         * @brief Converts std::tm to utc std::time_t.
         * @param tm Local time structure
         * @return Utc time_t value
         */
        inline std::time_t toUtcTimeT(const std::tm &tm) {
            std::tm tmCopy = tm;
#if defined(_WIN32) || defined(_WIN64)
            return _mkgmtime(&tmCopy);
#else
            return timegm(&tmCopy);
#endif
        }

        /**
         * @brief Gets the current system time in UTCZ.
         * @return Current time as a time stamp value in UTCZ
         */
        inline std::time_t getUtcNow() {
            auto now = std::chrono::system_clock::now();
            return std::chrono::system_clock::to_time_t(now);
        }

        /**
         * @brief Gets the current UTC timestamp as a string.
         * @param t Utc timestamp to convert, defaults to current time
         * @return String representation of the timestamp
         */
        inline std::string timeToString(std::time_t t = getUtcNow()) {
            return std::to_string(t);
        }

        /**
         * @brief Gets the current local time as a formatted string.
         * @param format Format string for strftime
         * @param utcT Utc timestamp to convert, defaults to current time
         * @return Formatted local time string, or std::nullopt on failure
         */
        inline std::optional<std::string> getLocalTimeString(std::string_view format = "%Y-%m-%d-%H-%M-%S", std::time_t utcT = getUtcNow()) {
            std::tm tmResult = toLocalTm(utcT);

            std::array<char, 128> timeString{};
            if (std::strftime(timeString.data(), timeString.size(), format.data(), &tmResult) > 0) {
                return std::string(timeString.data());
            }
            return std::nullopt;
        }

        /**
         * @brief Formats the current time as an ISO 8601 string in UTCZ.
         * @param utcT Utc timestamp to format, defaults to current time
         * @return Formatted time string in ISO 8601 format (e.g., "2024-06-07T15:04:05Z")
         */
        inline std::optional<std::string> getUtcZTimeString(std::time_t utcT = getUtcNow()) {
            std::tm tmResult = toUtcTm(utcT);

            std::array<char, 128> timeString{};
            if (std::strftime(timeString.data(), timeString.size(), "%Y-%m-%dT%H:%M:%SZ", &tmResult) > 0) {
                return std::string(timeString.data());
            }
            return std::nullopt;
        }

        /**
         * @brief Parses an ISO 8601 formatted string (e.g., "2024-06-07T15:04:05Z") to std::tm (UTC).
         * @param iso8601 ISO 8601 formatted string. format: "YYYY-MM-DDTHH:MM:SSZ" or "YYYY-MM-DDTHH:MM:SS+hh:mm" (included +hhmm/-hhmm)
         * @return std::optional<std::tm> Parsed UTC time structure, std::nullopt if parsing fails
         */
        inline std::optional<std::time_t> parseToUTCTime(const std::string &iso8601) {
            std::tm tmResult = {};
            std::regex r(R"((\d{4})-(\d{2})-(\d{2})[Tt](\d{2}):(\d{2}):(\d{2})(?:\.\d+)?(Z|([+-])(\d{2}):?(\d{2}))?)");
            std::smatch m;

            if (!std::regex_match(iso8601, m, r) || m.size() < 7) {
                return std::nullopt;
            }

            // At least require <YYYY>-<MM>-<DD>T/t<HH>:<MM>:<SS>Z = size 6(+1)
            tmResult.tm_year = std::stoi(m[1]) - 1900;
            tmResult.tm_mon = std::stoi(m[2]) - 1;
            tmResult.tm_mday = std::stoi(m[3]);
            tmResult.tm_hour = std::stoi(m[4]);
            tmResult.tm_min = std::stoi(m[5]);
            tmResult.tm_sec = std::stoi(m[6]);
            tmResult.tm_isdst = 0;

            // Zero timezone offset
            if (!m[7].matched || (m[7].matched && m[7] == "Z")) {
                return toUtcTimeT(tmResult);
            }

            // <YYYY>-<MM>-<DD>T/t<HH>:<MM>:<SS><+><hh>:<mm> = size 10(+1)
            if (m[8].matched && m[9].matched && m[10].matched) {
                int sign = (m[8] == "+") ? 1 : -1;
                int offset_hour = std::stoi(m[9]);
                int offset_min = std::stoi(m[10]);
                int offset_sec = sign * (offset_hour * 3600 + offset_min * 60);

                std::time_t t = toUtcTimeT(tmResult);
                // Subtract the timezone offset to get UTC time
                t -= offset_sec;
                tmResult = toUtcTm(t);
                return toUtcTimeT(tmResult);
            }

            return std::nullopt; // Unable to parse timezone offset
        }

        /**
         * @brief Parses an ISO 8601 formatted string (e.g., "2024-06-07T15:04:05Z") to a UTC 0 time string.
         * @param iso8601 ISO 8601 formatted string. format: "YYYY-MM-DDTHH:MM:SSZ" or "YYYY-MM-DDTHH:MM:SS+hh:mm" (included +hhmm/-hhmm)
         * @return std::optional<std::string> Parsed UTC 0 time string, std::nullopt if parsing fails
         */
        inline std::optional<std::string> parseToUTCTimeString(const std::string &iso8601) {
            auto utcTime = parseToUTCTime(iso8601);
            if (!utcTime) {
                return std::nullopt;
            }
            return getUtcZTimeString(utcTime.value());
        }

        /**
         * @brief Parses an ISO 8601 formatted string (e.g., "2024-06-07T15:04:05Z") to a local time string.
         * @param iso8601 ISO 8601 formatted string. format: "YYYY-MM-DDTHH:MM:SSZ" or "YYYY-MM-DDTHH:MM:SS+hh:mm" (included +hhmm/-hhmm)
         * @return std::optional<std::string> Parsed local time string, std::nullopt if parsing fails
         */
        inline std::optional<std::string> parseToLocalTimeString(const std::string &iso8601) {
            auto utcTime = parseToUTCTime(iso8601);
            if (!utcTime) {
                return std::nullopt;
            }
            return getLocalTimeString("%Y-%m-%d %H:%M:%S", utcTime.value());
        }
    } // namespace time

    /**
     * @namespace neko::util::base64
     * @brief Base64 encoding and decoding utilities.
     */
    namespace base64 {
        /**
         * @brief Characters used for Base64 encoding.
         */
        constexpr std::string_view base64Chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        /**
         * @brief Encodes a string using Base64.
         * @param input String to encode
         * @return Base64 encoded string
         */
        inline std::string base64Encode(const std::string &input) {
            std::string encodedString;
            int val = 0;
            int bits = -6;
            const unsigned int b63 = 0x3F;

            for (unsigned char c : input) {
                val = (val << 8) + c;
                bits += 8;
                while (bits >= 0) {
                    encodedString.push_back(base64Chars[(val >> bits) & b63]);
                    bits -= 6;
                }
            }

            if (bits > -6) {
                encodedString.push_back(base64Chars[((val << 8) >> (bits + 8)) & b63]);
            }

            while (encodedString.size() % 4) {
                encodedString.push_back('=');
            }

            return encodedString;
        }

        /**
         * @brief Decodes a Base64 encoded string.
         * @param input Base64 encoded string
         * @return Decoded string
         */
        inline std::string base64Decode(const std::string &input) {
            std::string decodedString;
            std::vector<int> base64Index(256, -1);
            for (int i = 0; i < 64; i++) {
                base64Index[base64Chars[i]] = i;
            }

            int val = 0;
            int bits = -8;
            for (unsigned char c : input) {
                if (base64Index[c] == -1)
                    break;
                val = (val << 6) + base64Index[c];
                bits += 6;

                if (bits >= 0) {
                    decodedString.push_back(char((val >> bits) & 0xFF));
                    bits -= 8;
                }
            }

            return decodedString;
        }
    } // namespace base64

    /**
     * @namespace neko::util::random
     * @brief Random value generation utilities.
     */
    namespace random {
        /**
         * @brief Generates a random hexadecimal string.
         * @param digits Number of hexadecimal digits to generate
         * @return Random hexadecimal string
         */
        inline std::string randomHex(int digits = 16) {
            static thread_local std::mt19937 gen{std::random_device{}()};
            static thread_local std::uniform_int_distribution<> dis(0, 15);
            const char *hex_chars = "0123456789abcdef";
            std::string result;
            result.reserve(digits);
            for (int i = 0; i < digits; ++i)
                result += hex_chars[dis(gen)];
            return result;
        }

        /**
         * @brief Generates a random number with a specified number of digits.
         * @param minimumDigits Minimum number of digits (default: 1)
         * @param maximumDigits Maximum number of digits (default: 10)
         * @return Random number
         */
        inline uint64_t randomNDigitNumber(int minimumDigits = 1, int maximumDigits = 10) {
            static thread_local std::mt19937 rng{std::random_device{}()};
            std::uniform_int_distribution<int> lenDist(minimumDigits, maximumDigits);
            int len = lenDist(rng);

            uint64_t min = static_cast<uint64_t>(std::pow(10, len - 1));
            uint64_t max = static_cast<uint64_t>(std::pow(10, len)) - 1;

            std::uniform_int_distribution<uint64_t> numDist(min, max);
            return numDist(rng);
        }

        /**
         * @brief Generates a random string of specified length.
         * @param length Length of the string to generate
         * @param characters Character set to use for generation
         * @return Random string
         * @throw std::invalid_argument if characters is empty or length is negative
         */
        inline std::string generateRandomString(
            int length,
            std::string_view characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
            if (characters.empty()) {
                throw std::invalid_argument("characters list must not be empty");
            }
            if (length < 0) {
                throw std::invalid_argument("length must be non-negative");
            }

            static thread_local std::mt19937 rng{std::random_device{}()};
            std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

            std::string randomString;
            randomString.reserve(length);

            for (int i = 0; i < length; ++i) {
                randomString += characters[distribution(rng)];
            }
            return randomString;
        }
    } // namespace random

    namespace check {
        /**
         * @brief Checks if a string is a valid URL.
         * @param str String to check
         * @return true if the string is a valid URL, false otherwise
         */
        inline bool isUrl(const std::string &str) {
            static const std::regex url_regex("(http|https)://[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{2,3}(/\\S*)?");
            return std::regex_match(str, url_regex);
        };

        /**
         * @brief Checks if a string is a valid proxy address.
         * @param address String to check
         * @return true if the string is a valid proxy address, false otherwise
         */
        inline bool isProxyAddress(const std::string &address) {
            static const std::regex proxyRegex(R"((http|https|socks5|socks4)://([\w.-]+)(:\d+))");
            return std::regex_match(address, proxyRegex);
        };

        struct ResolutionMatch {
            std::string full;   // "1920x1080"
            std::string width;  // "1920"
            std::string height; // "1080"
        };

        /**
         * @brief Extracts size specifications (e.g., "800x600") from a string.
         * @param str String to parse
         * @returns std::optional<ResolutionMatch> containing the resolution match if found.
         * @returns If no match is found, returns std::nullopt.
         */
        inline std::optional<ResolutionMatch> matchResolution(const std::string &str) {
            static const std::regex r(R"((\d{2,5})[xX](\d{2,5}))");
            std::smatch match;
            if (std::regex_search(str, match, r)) {
                if (match.size() == 3) {
                    return ResolutionMatch{
                        .full = match[0].str(),  // full match, e.g. "1920x1080"
                        .width = match[1].str(), // width
                        .height = match[2].str() // height
                    };
                }
            }
            return std::nullopt;
        }
    } // namespace check

} // namespace neko::util
