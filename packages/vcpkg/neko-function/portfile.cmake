vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO moehoshio/NekoFunction
    REF v1.0.0
    SHA512 482dc840e72fe515503f8d9870885b7a0aff16a0c3190d1ab813a4c6061cabf13dd8d1ddb04e431901221081b2e12dad5fbd73b33747665c56fa8c8f9dce498a
    HEAD_REF main
)

set(VCPKG_BUILD_TYPE release)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        archive  NEKO_FUNCTION_ENABLE_ARCHIVE
        hash     NEKO_FUNCTION_ENABLE_HASH
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DNEKO_FUNCTION_BUILD_TESTS=OFF
        -DNEKO_FUNCTION_AUTO_FETCH_DEPS=OFF
        -DNEKO_FUNCTION_ENABLE_MODULE=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/NekoFunction PACKAGE_NAME nekofunction)

# Only remove lib directory if both archive and hash features are disabled (header-only mode)
if(NOT "archive" IN_LIST FEATURES AND NOT "hash" IN_LIST FEATURES)
    file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib")
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
