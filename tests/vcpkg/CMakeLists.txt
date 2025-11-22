cmake_minimum_required(VERSION 3.14)

# Set policy for MSVC runtime library selection
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(NekoFunctionVcpkgTest VERSION 1.0 LANGUAGES CXX)

# Workaround for macOS vcpkg LibLZMA static library detection issue
# Disable LibLZMA requirement checks that fail with static libraries on macOS
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_DISABLE_FIND_PACKAGE_LibLZMA FALSE)
    # Try to find LibLZMA without strict function checks
    find_package(LibLZMA QUIET)
    if(LibLZMA_FOUND)
        # Override the REQUIRED components check
        set(LIBLZMA_HAS_AUTO_DECODER TRUE CACHE BOOL "" FORCE)
        set(LIBLZMA_HAS_EASY_ENCODER TRUE CACHE BOOL "" FORCE)
        set(LIBLZMA_HAS_LZMA_PRESET TRUE CACHE BOOL "" FORCE)
    endif()
endif()

# Find the NekoFunction package
find_package(NekoFunction REQUIRED CONFIG)

# Create test executable
add_executable(vcpkg_test test.cpp)

# Link against NekoFunction
target_link_libraries(vcpkg_test PRIVATE Neko::Function)

# Set C++20 standard
target_compile_features(vcpkg_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME vcpkg_test COMMAND vcpkg_test)
