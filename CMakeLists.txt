cmake_minimum_required(VERSION 3.20)
project(sri-race-plugins LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(BUILD_VERSION "" CACHE STRING "Version of the plugin being built")
if("${BUILD_VERSION}" STREQUAL "")
    message(SEND_ERROR "Missing or empty BUILD_VERSION argument")
endif()

set(CMAKE_MODULE_PATH /opt/race/race-cmake-modules)

# include(race/clang-format)
# include(race/coverage)
# include(race/lint)
# include(race/test-targets)
# include(race/valgrind)
# include(race/warnings)

# setup_project_test_targets(sri-race-plugins UNIT)
# setup_project_format_targets(sri-race-plugins)

add_subdirectory(source)

# if(NOT ANDROID)
# add_subdirectory(test/source EXCLUDE_FROM_ALL)
# else()
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#set(CMAKE_SKIP_BUILD_RPATH FALSE)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(CMAKE_INSTALL_RPATH "$ORIGIN")

# endif()

# setup_project_coverage_target()
