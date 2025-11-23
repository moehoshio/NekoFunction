cmake_minimum_required(VERSION 3.14)

# Set policy for MSVC runtime library selection
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(NekoFunctionVcpkgTest VERSION 1.0 LANGUAGES CXX)

# Workaround for vcpkg minizip-ng dependency issues
# On macOS: LibLZMA static library detection fails with vcpkg static libraries
# We need to pre-set the function check results before any find_package calls
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Pre-set LibLZMA function check results to bypass CHECK_LIBRARY_EXISTS failures
    # These are used by FindLibLZMA.cmake
    set(LIBLZMA_HAS_AUTO_DECODER TRUE CACHE BOOL "liblzma has lzma_auto_decoder" FORCE)
    set(LIBLZMA_HAS_EASY_ENCODER TRUE CACHE BOOL "liblzma has lzma_easy_encoder" FORCE)
    set(LIBLZMA_HAS_LZMA_PRESET TRUE CACHE BOOL "liblzma has lzma_lzma_preset" FORCE)
endif()

# Pre-find minizip-ng before NekoFunction to ensure the target is available
# This is needed because NekoFunctionTargets.cmake references MINIZIP::minizip-ng
# On macOS, this will now succeed because LibLZMA checks are pre-satisfied
find_package(minizip-ng QUIET CONFIG)

if(NOT minizip-ng_FOUND)
    message(STATUS "minizip-ng not found via CONFIG, trying MODULE mode")
    find_package(minizip-ng QUIET MODULE)
endif()

if(minizip-ng_FOUND)
    message(STATUS "minizip-ng found: ${minizip-ng_VERSION}")
else()
    message(WARNING "minizip-ng was not found. This may cause issues if NekoFunction was built with archiver support.")
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
