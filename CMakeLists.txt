cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0.11 LANGUAGES CXX)

# ================
# === Config ====
# ================

# NekoFunction project specific options
option(NEKO_FUNCTION_BUILD_TESTS "Neko Function Build tests" ON)
option(NEKO_FUNCTION_AUTO_FETCH_DEPS "Neko Function Automatically fetch dependencies" ON)

option(NEKO_FUNCTION_STATIC_LINK "Neko Function Static Link library" OFF)

option(NEKO_FUNCTION_ENABLE_ARCHIVE "Neko Function Enable archive support (requires minizip-ng)" ON)
option(NEKO_FUNCTION_ENABLE_HASH "Neko Function Enable hash support (requires OpenSSL)" ON)
option(NEKO_FUNCTION_ENABLE_MODULE "Neko Function Enable C++20 Module" OFF)


set(NEKO_FUNCTION_LIBRARY_PATH "" CACHE PATH "Path to look for dependencies (OpenSSL, minizip-ng..)")

if (NEKO_FUNCTION_LIBRARY_PATH)
    list(APPEND CMAKE_PREFIX_PATH "${NEKO_FUNCTION_LIBRARY_PATH}")
endif(NEKO_FUNCTION_LIBRARY_PATH)

find_package(NekoSchema QUIET)
find_package(minizip-ng QUIET)
find_package(OpenSSL QUIET)
find_package(GTest QUIET)


if (NOT ${OpenSSL_FOUND} AND NEKO_FUNCTION_ENABLE_HASH)
    message(WARNING "OpenSSL not found. Hash support will be disabled.")
    set(NEKO_FUNCTION_ENABLE_HASH OFF)
endif()

if (NOT ${minizip-ng_FOUND} AND NEKO_FUNCTION_ENABLE_ARCHIVE)
    message(WARNING "minizip-ng not found. Archive support will be disabled.")
    set(NEKO_FUNCTION_ENABLE_ARCHIVE OFF)
endif()

# Create a compatibility alias (in some package managers, the minizip-ng link name is minizip instead of minizip-ng)
if (NOT TARGET MINIZIP::minizip-ng AND TARGET MINIZIP::minizip)
    add_library(MINIZIP::minizip-ng ALIAS MINIZIP::minizip)
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
message(STATUS "  - Neko Function Enable Module: ${NEKO_FUNCTION_ENABLE_MODULE}")
message(STATUS "")
message(STATUS "Neko Function Dependency summary:")
message(STATUS "  - NekoSchema Found: ${NekoSchema_FOUND} version: ${NekoSchema_VERSION}")
message(STATUS "  - OpenSSL support: ${OpenSSL_FOUND} version: ${OpenSSL_VERSION}")
message(STATUS "  - minizip-ng support: ${minizip-ng_FOUND} version: ${minizip-ng_VERSION}")
message(STATUS "  - GTest : ${GTest_FOUND} version : ${GTest_VERSION}")
message(STATUS "  - Archive support enabled: ${NEKO_FUNCTION_ENABLE_ARCHIVE} via minizip-ng")
message(STATUS "  - Hash support enabled: ${NEKO_FUNCTION_ENABLE_HASH} via OpenSSL")
message(STATUS "")


if(NEKO_FUNCTION_AUTO_FETCH_DEPS)
    include(FetchContent)
    
    if (NOT NekoSchema_FOUND)
        message(STATUS "NekoSchema not found; Neko Function Fetching NekoSchema...")
        FetchContent_Declare(
            NekoSchema
            GIT_REPOSITORY https://github.com/moehoshio/NekoSchema.git
            GIT_TAG        main
        )
        set(NEKO_SCHEMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(NEKO_SCHEMA_ENABLE_MODULE ${NEKO_FUNCTION_ENABLE_MODULE} CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(NekoSchema)
    endif()
    

    if(NOT GTest_FOUND AND NEKO_FUNCTION_BUILD_TESTS)
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

if(NEKO_FUNCTION_ENABLE_ARCHIVE)

    # Create STATIC library with archive support
    add_library(NekoFunction STATIC src/neko/function/archiveZip.cpp)
    
    target_include_directories(NekoFunction PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(NekoFunction PUBLIC cxx_std_20)
    target_link_libraries(NekoFunction PUBLIC NekoSchema MINIZIP::minizip-ng)
    target_compile_definitions(NekoFunction PUBLIC NEKO_FUNCTION_ENABLE_ARCHIVE)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction PUBLIC NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        
        if (WIN32)
            get_target_property(_openssl_type_module OpenSSL::Crypto TYPE)
            get_target_property(_openssl_location_module OpenSSL::Crypto LOCATION)
            
            set(_is_static_openssl_module FALSE)
            if(_openssl_type_module STREQUAL "STATIC_LIBRARY")
                set(_is_static_openssl_module TRUE)
            elseif(_openssl_location_module AND _openssl_location_module MATCHES "\\.(a|lib)$")
                # Also check if the location ends with .a or .lib (static library extensions)
                set(_is_static_openssl_module TRUE)
            endif()
            
            if(_is_static_openssl_module OR NEKO_FUNCTION_STATIC_LINK)
                message(STATUS "Linking Windows system libraries for static OpenSSL")
                target_link_libraries(NekoFunction PUBLIC
                    ws2_32
                    Crypt32
                    advapi32
                    user32
                    bcrypt
                )
            endif()
        endif()
        
    endif(NEKO_FUNCTION_ENABLE_HASH)
    
else()

    # No translation unit create INTERFACE library
    add_library(NekoFunction INTERFACE)
    
    target_include_directories(NekoFunction INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_link_libraries(NekoFunction INTERFACE NekoSchema)
    target_compile_features(NekoFunction INTERFACE cxx_std_20)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction INTERFACE NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction INTERFACE OpenSSL::SSL OpenSSL::Crypto)
        
        if (WIN32)
            get_target_property(_openssl_type_module OpenSSL::Crypto TYPE)
            get_target_property(_openssl_location_module OpenSSL::Crypto LOCATION)
            
            set(_is_static_openssl_module FALSE)
            if(_openssl_type_module STREQUAL "STATIC_LIBRARY")
                set(_is_static_openssl_module TRUE)
            elseif(_openssl_location_module AND _openssl_location_module MATCHES "\\.(a|lib)$")
                # Also check if the location ends with .a or .lib (static library extensions)
                set(_is_static_openssl_module TRUE)
            endif()
            
            if(_is_static_openssl_module OR NEKO_FUNCTION_STATIC_LINK)
                message(STATUS "Linking Windows system libraries for static OpenSSL")
                target_link_libraries(NekoFunction INTERFACE
                    ws2_32
                    Crypt32
                    advapi32
                    user32
                    bcrypt
                )
            endif()
        endif()
        
    endif(NEKO_FUNCTION_ENABLE_HASH)

endif()

add_library(Neko::Function ALIAS NekoFunction)

# ================
# = C++20 Module =
# ================

if(NEKO_FUNCTION_ENABLE_MODULE)
    message(STATUS "NekoFunction C++20 module enabled (NEKO_FUNCTION_ENABLE_MODULE=ON)")
    
    # Check CMake version for module support
    if(CMAKE_VERSION VERSION_LESS 3.28)
        message(WARNING "CMake 3.28+ is recommended for full C++20 module support")
    endif()
    
    # Create module library
    add_library(NekoFunction_module)
    add_library(Neko::Function::Module ALIAS NekoFunction_module)
    
    set(NEKO_FUNCTION_MODULE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/neko/function/neko.function.cppm
    )
    
    # Add archive module if enabled
    if(NEKO_FUNCTION_ENABLE_ARCHIVE)
        list(APPEND NEKO_FUNCTION_MODULE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/neko/function/neko.function.archive.cppm
        )
    endif()
    
    target_sources(NekoFunction_module
        PUBLIC
            FILE_SET CXX_MODULES FILES
                ${NEKO_FUNCTION_MODULE_FILES}
    )
    
    target_compile_features(NekoFunction_module PUBLIC cxx_std_20)
    
    # Link dependencies (needed for module compilation)
    target_link_libraries(NekoFunction_module PUBLIC NekoSchema_module)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction_module PUBLIC NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction_module PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        
        if (WIN32)
            get_target_property(_openssl_type_module OpenSSL::Crypto TYPE)
            get_target_property(_openssl_location_module OpenSSL::Crypto LOCATION)
            
            set(_is_static_openssl_module FALSE)
            if(_openssl_type_module STREQUAL "STATIC_LIBRARY")
                set(_is_static_openssl_module TRUE)
            elseif(_openssl_location_module AND _openssl_location_module MATCHES "\\.(a|lib)$")
                # Also check if the location ends with .a or .lib (static library extensions)
                set(_is_static_openssl_module TRUE)
            endif()
            
            if(_is_static_openssl_module OR NEKO_FUNCTION_STATIC_LINK)
                message(STATUS "Linking Windows system libraries for static OpenSSL (module)")
                target_link_libraries(NekoFunction_module PUBLIC
                    ws2_32
                    Crypt32
                    advapi32
                    user32
                    bcrypt
                )
            endif()
        endif()
        
    endif(NEKO_FUNCTION_ENABLE_HASH)

    # Archive support
    if(NEKO_FUNCTION_ENABLE_ARCHIVE)
        target_compile_definitions(NekoFunction_module PUBLIC NEKO_FUNCTION_ENABLE_ARCHIVE)
        target_link_libraries(NekoFunction_module PUBLIC MINIZIP::minizip-ng)
    endif(NEKO_FUNCTION_ENABLE_ARCHIVE)
    
    # Module still needs access to the include directory for the headers
    target_include_directories(NekoFunction_module PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    
else()
    message(STATUS "NekoFunction C++20 module disabled (NEKO_FUNCTION_ENABLE_MODULE=OFF)")
endif()

# ================
# ==== Tests =====
# ================

if(NEKO_FUNCTION_BUILD_TESTS)
    enable_testing()
    message(STATUS "NekoFunction tests enabled (NEKO_FUNCTION_BUILD_TESTS=ON)")

    if (NOT GTEST_FOUND AND NOT NEKO_FUNCTION_AUTO_FETCH_DEPS)
        message(WARNING "GTest is required for building tests but was not found.")
        message(FATAL_ERROR "Please enable -DNEKO_FUNCTION_AUTO_FETCH_DEPS=ON or install GTest and make it discoverable by CMake. use -DNEKO_FUNCTION_LIBRARY_PATH=</path/to/gtest>")
    endif()
    
    # Ensure consistent runtime library usage on Windows
    if(MSVC)
        if(NEKO_FUNCTION_STATIC_LINK OR VCPKG_TARGET_TRIPLET MATCHES ".*-static$")
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            message(STATUS "Using static MSVC runtime library (/MT)")
        else()
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
            message(STATUS "Using dynamic MSVC runtime library (/MD)")
        endif()
    endif()
    

    add_executable(NekoFunction_test tests/function_test.cpp)
    target_include_directories(NekoFunction_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(NekoFunction_test PRIVATE NekoFunction GTest::gtest GTest::gtest_main)
    target_compile_features(NekoFunction_test PRIVATE cxx_std_20)
    
    add_test(NAME NekoFunction_tests COMMAND NekoFunction_test)

    include(GoogleTest)
    gtest_discover_tests(NekoFunction_test)

    # Module test (if modules are enabled)
    if(NEKO_FUNCTION_ENABLE_MODULE)
        if (MSVC)
            message(STATUS "NekoFunction module tests enabled (MSVC only)")

            add_executable(NekoFunction_module_tests tests/function_module_test.cpp)
            target_link_libraries(NekoFunction_module_tests PRIVATE Neko::Schema::Module Neko::Function::Module GTest::gtest GTest::gtest_main)
            target_compile_features(NekoFunction_module_tests PRIVATE cxx_std_20)
            if(WIN32 AND NEKO_FUNCTION_ENABLE_HASH)
                target_link_libraries(NekoFunction_module_tests PRIVATE ws2_32 Crypt32 advapi32 user32 bcrypt)
            endif()
            gtest_discover_tests(NekoFunction_module_tests)
        else()
            message(STATUS "Skipping NekoFunction module tests on non-MSVC toolchains")
        endif()
    endif(NEKO_FUNCTION_ENABLE_MODULE)

else()
    message(STATUS "NekoFunction tests disabled (NEKO_FUNCTION_BUILD_TESTS=OFF)")
endif()


# ================
# == Install =====
# ================

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
    PATTERN "*.hpp"
    PATTERN "*.cppm"
)

# Install targets
install(TARGETS NekoFunction
    EXPORT NekoFunctionTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install export targets
install(EXPORT NekoFunctionTargets
    FILE NekoFunctionTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NekoFunction
)

# Create and install Config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NekoFunctionConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NekoFunctionConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NekoFunction
)

# Create and install Version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NekoFunctionConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NekoFunctionConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NekoFunctionConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NekoFunction
)

message(STATUS "Neko Function End of configuration")