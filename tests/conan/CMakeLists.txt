cmake_minimum_required(VERSION 3.14)
project(NekoFunctionConanTest VERSION 1.0 LANGUAGES CXX)

# Find the NekoFunction package
find_package(NekoFunction REQUIRED CONFIG)

# Create test executable
add_executable(conan_test test.cpp)

# Link against NekoFunction
target_link_libraries(conan_test PRIVATE Neko::Function)

# Set C++20 standard
target_compile_features(conan_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME conan_test COMMAND conan_test)
