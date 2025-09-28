cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0 LANGUAGES CXX)

option(NEKO_AUTO_FETCH_DEPS "Automatically fetch dependencies" ON)
option(NEKO_BUILD_TESTS "Build tests" ON)
option(NEKO_ENABLE_ARCHIVER "Enable archiver support (requires minizip-ng)" ON)

find_package(minizip-ng CONFIG QUIET)
find_package(OpenSSL QUIET)

set(NEKOFUNCTION_SOURCES)

if(NEKO_AUTO_FETCH_DEPS)
    include(FetchContent)
    FetchContent_Declare(
        NekoSchema
        GIT_REPOSITORY https://github.com/moehoshio/NekoSchema.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(NekoSchema)
endif()

# Main library target
if(MINIZIP_FOUND AND NEKO_ENABLE_ARCHIVER)
    add_library(NekoFunction STATIC src/neko/function/archiverZip.cpp)
else()
    add_library(NekoFunction INTERFACE)
endif()
add_library(Neko::Function ALIAS NekoFunction)



target_include_directories(NekoFunction INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(OpenSSL_FOUND)
    target_compile_definitions(NekoFunction INTERFACE NEKO_ENABLE_HASH_SUPPORT)
    target_link_libraries(NekoFunction INTERFACE OpenSSL::SSL OpenSSL::Crypto)
endif()

if(MINIZIP_FOUND AND NEKO_ENABLE_ARCHIVER)
    target_compile_definitions(NekoFunction INTERFACE NEKO_ENABLE_ARCHIVER)
    target_link_libraries(NekoFunction INTERFACE MINIZIP::minizip-ng)
endif()

target_link_libraries(NekoFunction INTERFACE NekoSchema)
target_compile_features(NekoFunction INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(NekoFunction INTERFACE /Zc:__cplusplus)
endif()

if(NEKO_BUILD_TESTS)
    enable_testing()
    
    # Create test executable
    add_executable(NekoFunction_test tests/NekoFunction_test.cpp)
    target_include_directories(NekoFunction_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(NekoFunction_test PRIVATE NekoFunction)
    target_compile_features(NekoFunction_test PRIVATE cxx_std_20)
    
    if(OpenSSL_FOUND)
        target_link_libraries(NekoFunction_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    endif()
    
    if(MSVC)
        target_compile_options(NekoFunction_test PRIVATE /Zc:__cplusplus)
    endif()

    # Add the test first
    add_test(NAME NekoFunction_tests COMMAND NekoFunction_test)

    set_tests_properties(NekoFunction_tests PROPERTIES
        TIMEOUT 120
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Print configuration summary for tests
    message(STATUS "NekoFunction tests enabled")
    
    message(STATUS "Test configuration summary:")
    message(STATUS "  - OpenSSL support: ${OpenSSL_FOUND}")
    message(STATUS "  - Archiver support: ${minizip-ng_FOUND}")
    message(STATUS "  - Hash support enabled: ${OpenSSL_FOUND}")
    message(STATUS "  - Archiver enabled: ${NEKO_ENABLE_ARCHIVER}")
    
else()
    message(STATUS "NekoFunction tests disabled (NEKO_BUILD_TESTS=OFF)")
endif()