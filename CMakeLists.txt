cmake_minimum_required(VERSION 3.14)
project(NekoFunction VERSION 1.0 LANGUAGES CXX)

option(NEKO_AUTO_FETCH_DEPS "Automatically fetch dependencies" ON)

option(NEKO_BUILD_TESTS "Build tests" ON)

if(NEKO_AUTO_FETCH_DEPS)

    include(FetchContent)

    FetchContent_Declare(
        NekoSchema
        GIT_REPOSITORY https://github.com/moehoshio/NekoSchema.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(NekoSchema)

endif(NEKO_AUTO_FETCH_DEPS)


add_library(NekoFunction INTERFACE)

target_include_directories(NekoFunction INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Dependencies
target_link_libraries(NekoFunction INTERFACE NekoSchema)

target_compile_features(NekoFunction INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(NekoFunction INTERFACE /Zc:__cplusplus)
endif()
