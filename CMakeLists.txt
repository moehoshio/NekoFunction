cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0.0 LANGUAGES CXX)

# ================
# === Config ====
# ================

# NekoFunction project specific options
option(NEKO_FUNCTION_BUILD_TESTS "Neko Function Build tests" ON)
option(NEKO_FUNCTION_AUTO_FETCH_DEPS "Neko Function Automatically fetch dependencies" ON)

option(NEKO_FUNCTION_STATIC_LINK "Neko Function Static Link library" OFF)

option(NEKO_FUNCTION_ENABLE_ARCHIVER "Neko Function Enable archiver support (requires minizip-ng)" ON)
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
message(STATUS "  - NekoSchema Found: ${NekoSchema_FOUND} version: ${NekoSchema_VERSION}")
message(STATUS "  - OpenSSL support: ${OpenSSL_FOUND} version: ${OpenSSL_VERSION}")
message(STATUS "  - minizip-ng support: ${minizip-ng_FOUND} version: ${minizip-ng_VERSION}")
message(STATUS "  - GTest : ${GTest_FOUND} version : ${GTest_VERSION}")
message(STATUS "  - Archiver support enabled: ${NEKO_FUNCTION_ENABLE_ARCHIVER} via minizip-ng")
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
        if (NEKO_FUNCTION_ENABLE_MODULE)
            set(NEKO_SCHEMA_ENABLE_MODULE ON CACHE BOOL "" FORCE)
        endif()
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

if(NEKO_FUNCTION_ENABLE_ARCHIVER OR NEKO_FUNCTION_ENABLE_HASH)

    # Create STATIC library with optional features
    add_library(NekoFunction STATIC src/neko/function/archiverZip.cpp)
    
    target_include_directories(NekoFunction PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(NekoFunction PUBLIC cxx_std_20)
    target_link_libraries(NekoFunction PUBLIC NekoSchema)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction PUBLIC NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        
        # static link on Windows
        if (NEKO_FUNCTION_STATIC_LINK AND WIN32)
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
    
    target_sources(NekoFunction_module
        PUBLIC
            FILE_SET CXX_MODULES FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/include/neko/function/neko.function.cppm
    )
    
    target_compile_features(NekoFunction_module PUBLIC cxx_std_20)
    
    # Link dependencies (needed for module compilation)
    target_link_libraries(NekoFunction_module PUBLIC NekoSchema_module)

    # Hash support
    if (NEKO_FUNCTION_ENABLE_HASH)
        target_compile_definitions(NekoFunction_module PUBLIC NEKO_FUNCTION_ENABLE_HASH NEKO_IMPORT_OPENSSL)
        target_link_libraries(NekoFunction_module PUBLIC OpenSSL::SSL OpenSSL::Crypto)
        
        # static link on Windows
        if (NEKO_FUNCTION_STATIC_LINK AND WIN32)
            target_link_libraries(NekoFunction_module PUBLIC
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
        target_compile_definitions(NekoFunction_module PUBLIC NEKO_FUNCTION_ENABLE_ARCHIVER)
        target_link_libraries(NekoFunction_module PUBLIC MINIZIP::minizip-ng)
    endif(NEKO_FUNCTION_ENABLE_ARCHIVER)
    
    # Compiler-specific module flags
    if(MSVC)
        target_compile_options(NekoFunction_module PUBLIC /experimental:module)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(NekoFunction_module PUBLIC -fmodules-ts)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(NekoFunction_module PUBLIC -fmodules)
    endif()
    
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
    

    add_executable(NekoFunction_test tests/function_test.cpp)
    target_include_directories(NekoFunction_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(NekoFunction_test PRIVATE NekoFunction GTest::gtest GTest::gtest_main)
    target_compile_features(NekoFunction_test PRIVATE cxx_std_20)
    
    add_test(NAME NekoFunction_tests COMMAND NekoFunction_test)

    include(GoogleTest)
    gtest_discover_tests(NekoFunction_test)

    if (NEKO_FUNCTION_ENABLE_MODULE)
        add_executable(NekoFunction_module_tests tests/function_module_test.cpp)
        target_link_libraries(NekoFunction_module_tests PRIVATE Neko::Schema::Module Neko::Function::Module GTest::gtest GTest::gtest_main)
        target_compile_features(NekoFunction_module_tests PRIVATE cxx_std_20)
        gtest_discover_tests(NekoFunction_module_tests)
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