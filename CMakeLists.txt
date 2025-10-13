cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0 LANGUAGES CXX)

# Neko Module common options
option(NEKO_AUTO_FETCH_DEPS "Neko Automatically fetch dependencies" ON)
option(NEKO_BUILD_TESTS "Neko Build tests" ON)

# NekoFunction project specific options
option(NEKO_FUNCTION_BUILD_TESTS "Neko Function Build tests" ON)
option(NEKO_FUNCTION_AUTO_FETCH_DEPS "Neko Function Automatically fetch dependencies" ON)

option(NEKO_FUNCTION_ENABLE_ARCHIVER "Neko Function Enable archiver support (requires minizip-ng)" ON)

find_package(minizip-ng CONFIG QUIET)
find_package(OpenSSL QUIET)

# Print configuration summary
message(STATUS "Start configuration Neko Function")
message(STATUS "NekoFunction configuration summary:")
message(STATUS "  - CMake version: ${CMAKE_VERSION}")
message(STATUS "")
message(STATUS "  - Neko Auto fetch deps: ${NEKO_AUTO_FETCH_DEPS}")
message(STATUS "  - Neko Build tests: ${NEKO_BUILD_TESTS}")
message(STATUS "  - Neko Archiver enabled: ${NEKO_ENABLE_ARCHIVER}")
message(STATUS "")
message(STATUS "Dependency summary:")
message(STATUS "  - GoogleTest support: ${GTest_FOUND}")
message(STATUS "  - OpenSSL support: ${OpenSSL_FOUND}")
message(STATUS "  - Archiver and minizip-ng support: ${minizip-ng_FOUND}")
message(STATUS "  - Hash support enabled: ${OpenSSL_FOUND}")
message(STATUS "")


if(NEKO_AUTO_FETCH_DEPS OR NEKO_FUNCTION_AUTO_FETCH_DEPS)
    include(FetchContent)
    
    FetchContent_Declare(
        NekoSchema
        GIT_REPOSITORY https://github.com/moehoshio/NekoSchema.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(NekoSchema)

    if(NEKO_BUILD_TESTS OR NEKO_FUNCTION_BUILD_TESTS)
        message(STATUS "GTest not found. Fetching GoogleTest...")

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        release-1.12.1
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

endif()

# Main library target
if(minizip-ng_FOUND AND NEKO_FUNCTION_ENABLE_ARCHIVER)
    add_library(NekoFunction STATIC src/neko/function/archiverZip.cpp)
else()
    add_library(NekoFunction INTERFACE)
endif()
add_library(Neko::Function ALIAS NekoFunction)

target_include_directories(NekoFunction INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(NekoFunction INTERFACE NekoSchema)
target_compile_features(NekoFunction INTERFACE cxx_std_20)

if(OpenSSL_FOUND)
    target_compile_definitions(NekoFunction INTERFACE NEKO_ENABLE_HASH_SUPPORT)
    target_link_libraries(NekoFunction INTERFACE OpenSSL::SSL OpenSSL::Crypto)
    if(WIN32)
        if(minizip-ng_FOUND AND NEKO_FUNCTION_ENABLE_ARCHIVER)
            # STATIC library - use PUBLIC
            target_link_libraries(NekoFunction PUBLIC
                Crypt32
                ws2_32
                advapi32
                user32
                bcrypt
            )
        else()
            # INTERFACE library - use INTERFACE
            target_link_libraries(NekoFunction INTERFACE
                Crypt32
                ws2_32
                advapi32
                user32
                bcrypt
            )
        endif()
        message(STATUS "Linking Windows system libraries for OpenSSL static build")
    endif()
endif()

if(minizip-ng_FOUND AND NEKO_FUNCTION_ENABLE_ARCHIVER)
    target_compile_definitions(NekoFunction INTERFACE NEKO_FUNCTION_ENABLE_ARCHIVER)
    target_link_libraries(NekoFunction INTERFACE MINIZIP::minizip-ng)
endif()

if(MSVC)
    target_compile_options(NekoFunction INTERFACE /Zc:__cplusplus)
endif()


# Tests
if(NEKO_BUILD_TESTS OR NEKO_FUNCTION_BUILD_TESTS)
    enable_testing()
    message(STATUS "NekoFunction tests enabled")

    if (NOT NEKO_AUTO_FETCH_DEPS AND NOT NEKO_FUNCTION_AUTO_FETCH_DEPS)
        message(FATAL_ERROR "GTest is required for building tests. Please enable NEKO_AUTO_FETCH_DEPS")
    endif()

    add_executable(NekoFunction_test tests/NekoFunction_test.cpp)
    target_include_directories(NekoFunction_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(NekoFunction_test PRIVATE NekoFunction GTest::gtest_main)
    target_compile_features(NekoFunction_test PRIVATE cxx_std_20)
    
    add_test(NAME NekoFunction_tests COMMAND NekoFunction_test)

    include(GoogleTest)
    gtest_discover_tests(NekoFunction_test)
else()
    message(STATUS "NekoFunction tests disabled (NEKO_BUILD_TESTS=OFF)")
endif()

message(STATUS "End Neko Function")