cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0 LANGUAGES CXX)

# ================
# === Config ====
# ================

# NekoFunction project specific options
option(NEKO_FUNCTION_BUILD_TESTS "Neko Function Build tests" ON)
option(NEKO_FUNCTION_AUTO_FETCH_DEPS "Neko Function Automatically fetch dependencies" ON)

option(NEKO_FUNCTION_STATIC_LINK "Neko Function Static Link library" OFF)

option(NEKO_FUNCTION_ENABLE_ARCHIVER "Neko Function Enable archiver support (requires minizip-ng)" ON)
option(NEKO_FUNCTION_ENABLE_HASH "Neko Function Enable hash support (requires OpenSSL)" ON)


set(NEKO_FUNCTION_LIBRARY_PATH "" CACHE PATH "Path to look for dependencies (OpenSSL, minizip-ng)")

if (NEKO_FUNCTION_LIBRARY_PATH)
    list(APPEND CMAKE_PREFIX_PATH "${NEKO_FUNCTION_LIBRARY_PATH}")
endif(NEKO_FUNCTION_LIBRARY_PATH)

find_package(minizip-ng QUIET)
find_package(OpenSSL QUIET)
find_package(GTest QUIET)


if (NOT ${OpenSSL_FOUND} AND NEKO_FUNCTION_ENABLE_HASH)
    message(WARNING "OpenSSL not found. Hash support will be disabled.")
    set(NEKO_FUNCTION_ENABLE_HASH OFF)
endif()

if (NOT ${minizip-ng_FOUND} AND NEKO_FUNCTION_ENABLE_ARCHIVER)
    message(WARNING "minizip-ng not found. Archiver support will be disabled.")
    set(NEKO_FUNCTION_ENABLE_ARCHIVER OFF)
endif()

# Print configuration summary
message(STATUS "Start configuration Neko Function...")
message(STATUS "")
message(STATUS "Neko Function configuration summary:")
message(STATUS "  - CMake version: ${CMAKE_VERSION}")
message(STATUS "  - CMake Prefix Path: ${CMAKE_PREFIX_PATH}")
message(STATUS "  - CMake Module Path: ${CMAKE_MODULE_PATH}")
message(STATUS "  - Neko Function Library Path: ${NEKO_FUNCTION_LIBRARY_PATH}")
message(STATUS "  - Neko Function Static Link: ${NEKO_FUNCTION_STATIC_LINK}")
message(STATUS "")
message(STATUS "  - Neko Function Auto fetch deps: ${NEKO_FUNCTION_AUTO_FETCH_DEPS}")
message(STATUS "  - Neko Function Build tests: ${NEKO_FUNCTION_BUILD_TESTS}")
message(STATUS "")
message(STATUS "Neko Function Dependency summary:")
message(STATUS "  - OpenSSL support: ${OpenSSL_FOUND} version: ${OpenSSL_VERSION}")
message(STATUS "  - minizip-ng support: ${minizip-ng_FOUND} version: ${minizip-ng_VERSION}")
message(STATUS "  - GTest : ${GTest_FOUND} version : ${GTest_VERSION}")
message(STATUS "  - Archiver support enabled: ${NEKO_FUNCTION_ENABLE_ARCHIVER} via minizip-ng")
message(STATUS "  - Hash support enabled: ${NEKO_FUNCTION_ENABLE_HASH} via OpenSSL")
message(STATUS "")


if(NEKO_FUNCTION_AUTO_FETCH_DEPS)
    include(FetchContent)
    
    FetchContent_Declare(
        NekoSchema
        GIT_REPOSITORY https://github.com/moehoshio/NekoSchema.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(NekoSchema)

    if(NOT GTEST_FOUND AND NEKO_FUNCTION_BUILD_TESTS)
        message(STATUS "GTest not found; Neko Function Fetching GoogleTest...")

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.17.0
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

# ================
# = Main target =
# ================

if(NEKO_FUNCTION_ENABLE_ARCHIVER OR NEKO_FUNCTION_ENABLE_HASH)

    add_library(NekoFunction STATIC src/neko/function/archiverZip.cpp)
    
    target_include_directories(NekoFunction PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_features(NekoFunction PUBLIC cxx_std_20)
    target_link_libraries(NekoFunction PUBLIC NekoSchema)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction PUBLIC NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        
        # static link on Windows
        if (NEKO_FUNCTION_STATIC_LINK AND WIN32)
            target_compile_definitions(NekoFunction PUBLIC OPENSSL_USE_STATIC_LIBS)
            target_link_libraries(NekoFunction PUBLIC
            Crypt32
            advapi32
            ws2_32
            user32
            bcrypt
        )
        endif(NEKO_FUNCTION_STATIC_LINK AND WIN32)
        
    endif(NEKO_FUNCTION_ENABLE_HASH)

    # Archiver support
    if(NEKO_FUNCTION_ENABLE_ARCHIVER)
        target_compile_definitions(NekoFunction PUBLIC NEKO_FUNCTION_ENABLE_ARCHIVER)
        target_link_libraries(NekoFunction PUBLIC MINIZIP::minizip-ng)
    endif(NEKO_FUNCTION_ENABLE_ARCHIVER)
    
else()

    # No optional features enabled, create INTERFACE library
    add_library(NekoFunction INTERFACE)
    
    target_include_directories(NekoFunction INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(NekoFunction INTERFACE NekoSchema)
    target_compile_features(NekoFunction INTERFACE cxx_std_20)

endif()

add_library(Neko::Function ALIAS NekoFunction)

if(MSVC)
    target_compile_options(NekoFunction INTERFACE /Zc:__cplusplus)
endif()


# ================
# ==== Tests =====
# ================

if(NEKO_FUNCTION_BUILD_TESTS)
    enable_testing()
    message(STATUS "NekoFunction tests enabled (NEKO_FUNCTION_BUILD_TESTS=ON)")

    if (NOT GTEST_FOUND AND NOT NEKO_FUNCTION_AUTO_FETCH_DEPS)
        message(STATUS "GTest is required for building tests but was not found.")
        message(FATAL_ERROR "Please enable -DNEKO_FUNCTION_AUTO_FETCH_DEPS=ON or install GTest and make it discoverable by CMake. e.g -DCMAKE_PREFIX_PATH=</path/to/googletest>")
    endif()
    

    add_executable(NekoFunction_test tests/NekoFunction_test.cpp)
    target_include_directories(NekoFunction_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(NekoFunction_test PRIVATE NekoFunction GTest::gtest GTest::gtest_main)
    target_compile_features(NekoFunction_test PRIVATE cxx_std_20)
    
    add_test(NAME NekoFunction_tests COMMAND NekoFunction_test)

    include(GoogleTest)
    gtest_discover_tests(NekoFunction_test)
else()
    message(STATUS "NekoFunction tests disabled (NEKO_FUNCTION_BUILD_TESTS=OFF)")
endif()

message(STATUS "Neko Function End of configuration")