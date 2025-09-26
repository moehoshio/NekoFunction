cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0 LANGUAGES CXX)

option(NEKO_AUTO_FETCH_DEPS "Automatically fetch dependencies" ON)
option(NEKO_BUILD_TESTS "Build tests" ON)
option(NEKO_ENABLE_ARCHIVER "Enable archiver support (requires minizip-ng)" ON)

find_package(minizip-ng CONFIG QUIET)
find_package(OpenSSL QUIET)

set(NEKOFUNCTION_SOURCES)

if(MINIZIP_FOUND AND NEKO_ENABLE_ARCHIVER)
    message(STATUS "minizip-ng found: ${MINIZIP_VERSION}")
    list(APPEND NEKOFUNCTION_SOURCES src/neko/function/archiverZip.cpp)
endif()

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
add_library(NekoFunction INTERFACE ${NEKOFUNCTION_SOURCES})
add_library(Neko::Function ALIAS NekoFunction)

target_include_directories(NekoFunction PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(OpenSSL_FOUND)
    target_compile_definitions(NekoFunction PUBLIC NEKO_ENABLE_HASH_SUPPORT)
endif()

if(MINIZIP_FOUND AND NEKO_ENABLE_ARCHIVER)
    target_compile_definitions(NekoFunction PUBLIC NEKO_ENABLE_ARCHIVER)
    target_link_libraries(NekoFunction PUBLIC MINIZIP::minizip-ng)
endif()

target_link_libraries(NekoFunction PUBLIC NekoSchema)
target_compile_features(NekoFunction PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(NekoFunction PRIVATE /Zc:__cplusplus)
endif()