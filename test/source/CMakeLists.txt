# Setup Packages
find_package(CURL)
include_directories(${CURL_INCLUDE_DIR})

# Set warning flags for the compiler
# include(../../source/warnings.cmake.txt)

add_executable(unitTestPluginTa2DecomposedCpp
    ../../source/common/log.cpp
    ../../source/encoding/PluginTa2TwoSixStubEncoding.cpp
    ../../source/flickr_transport/FlickrLink.cpp
    ../../source/flickr_transport/LinkAddress.cpp
    ../../source/flickr_transport/LinkMap.cpp
    ../../source/flickr_transport/MessageHashQueue.cpp
    ../../source/flickr_transport/PluginTa2SRIFlickrTransport.cpp
    ../../source/flickr_transport/PluginTa2TwoSixStubTransport.cpp
    ../../source/flickr_transport/flickr_transport.c
    ../../source/user-model/LinkUserModel.cpp
    ../../source/user-model/MarkovModel.cpp
    ../../source/user-model/PluginTa2TwoSixStubUserModel.cpp

    main.cpp
    transport/TestLink.cpp
    transport/TestLinkMap.cpp
    transport/TestPluginTa2TwoSixStubTransport.cpp
    user-model/TestLinkUserModel.cpp
    user-model/TestMarkovModel.cpp
    user-model/TestPluginTa2TwoSixStubUserModel.cpp
)

# Disable compiling implementations for createPluginTa2 and destroyPluginTa2
target_compile_definitions(unitTestPluginTa2DecomposedCpp PUBLIC TESTBUILD JSON_DIAGNOSTICS=1)

target_include_directories(unitTestPluginTa2DecomposedCpp PRIVATE
    common/
    ../../source/common/
    ../../source/encoding/
    ../../source/flickr_transport/
    ../../source/user-model/
)

find_package(Threads REQUIRED)

find_library(LIB_CRYPTO crypto)
find_library(LIB_SSL ssl)

target_link_libraries(unitTestPluginTa2DecomposedCpp
    gmock
    gtest
    raceSdkCommon
    Threads::Threads
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    stdc++fs
    cpprest
    ${LIB_CRYPTO}
    ${LIB_SSL}
)

if (TARGET raceSdkTestMocks)
    target_link_libraries(unitTestPluginTa2DecomposedCpp raceSdkTestMocks)
endif()

add_dependencies(build_sri-race-plugins_tests unitTestPluginTa2DecomposedCpp)
add_test(sri-race-plugins ${CMAKE_CURRENT_BINARY_DIR}/unitTestPluginTa2DecomposedCpp)
set_tests_properties(sri-race-plugins PROPERTIES LABELS "unit;sri-race-plugins")
setup_coverage_for_target(
    TARGET unitTestPluginTa2DecomposedCpp
    SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../..
)
setup_valgrind_for_target(unitTestPluginTa2DecomposedCpp)
setup_clang_format_for_target(unitTestPluginTa2DecomposedCpp PARENT sri-race-plugins)
