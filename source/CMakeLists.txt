################################################################################
# Standard library dependencies (common to all components)
################################################################################

# if(ANDROID)
#     find_library(LIB_CURL curl)
#     find_library(LIB_XML2 xml)
#     find_package(Boost COMPONENTS filesystem system REQUIRED) 
#     find_library(LIB_CPPREST cpprest)
#     find_library(LIB_JSONCPP jsoncpp)
#     find_library(LIB_EVENT event)
#     find_library(LIB_LOG log)
#     find_library(LIB_FLICKCURL flickcurl)
#     find_library(RACE_JAVA_SHIMS RaceJavaShims)
#     find_library(LIB_SDK_COMMON raceSdkCommon)

#     set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#     set(CMAKE_SKIP_BUILD_RPATH FALSE)
#     set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#     set(CMAKE_INSTALL_RPATH "$ORIGIN")

#     set(LIB_DEPS
#         ${LIB_SDK_COMMON}
#         ${LIB_CURL}
# 	${LIB_XML2}
#         Boost::system
#         Boost::filesystem
#         ${LIB_CPPREST}
#         #${LIB_JSONCPP}
#         ${LIB_EVENT}
#         ${LIB_LOG}
# 	${LIB_FLICKCURL}
#         ${RACE_JAVA_SHIMS}
#     )
# else()
    ## Default to Linux
    find_package(CURL)
    find_package(Boost COMPONENTS filesystem system REQUIRED) 
    find_library(LIB_XML2 xml2)
    find_library(LIB_CPPREST cpprest)
    find_library(LIB_CRYPTO crypto)
    find_library(LIB_SSL ssl)
    find_library(LIB_JSONCPP jsoncpp)
    find_library(LIB_EVENT event)
    find_library(LIB_FLICKCURL flickcurl)
    # include_directories(${CURL_INCLUDE_DIR} ../racesdk/common/include ../racesdk/core/include )
    # if ("${TARGET_ARCHITECTURE}" STREQUAL "LINUX_arm64-v8a")
    # 	    include_directories(${CURL_INCLUDE_DIR} /linux/arm64-v8a/include )
    # else()
    # 	    include_directories(${CURL_INCLUDE_DIR} /linux/x86_64/include )
    # endif()

    message( "Found environment variable LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}" )

    set(LIB_DEPS
        # raceSdkCommon
        ${CURL_LIBRARIES}
        Boost::system
        Boost::filesystem
        ${LIB_CPPREST}
        ${LIB_CRYPTO}
        ${LIB_SSL}
        ${LIB_JSONCPP}
        ${LIB_EVENT}
	${LIB_FLICKCURL}
	${LIB_XML2}
    )
    message( "LIB_FLICKCURL: ${LIB_FLICKCURL}" )

# endif()

################################################################################
# Function to define the component CMake target
################################################################################

set(PLUGIN_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../kit)
set(COMMON_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS ${COMMON_SRC_DIR}/*.h)

function(setup_component_target)
    set(OPTIONS "")
    set(ONE_VALUE_ARGS TARGET)
    set(MULTI_VALUE_ARGS SOURCES)
    cmake_parse_arguments(COMPONENT "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    add_library(${COMPONENT_TARGET} SHARED ${COMPONENT_SOURCES})
    target_include_directories(${COMPONENT_TARGET} PRIVATE ${COMMON_SRC_DIR})
    target_link_libraries(${COMPONENT_TARGET} ${LIB_DEPS})
    target_compile_definitions(${COMPONENT_TARGET} PUBLIC
        BUILD_VERSION="${BUILD_VERSION}"
    )
    set_target_properties(${COMPONENT_TARGET} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        NO_SONAME ON
    )

    file(GLOB_RECURSE COMP_HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
    list(APPEND COMP_HEADERS "${COMMON_HEADERS}")
    set_target_properties(${COMPONENT_TARGET} PROPERTIES
        PRIVATE_HEADER "${COMP_HEADERS}"
    )

    # TODO enable performance-* and readability-* checks (needs SDK fixes)
    # setup_clang_tidy_for_target(TARGET ${COMPONENT_TARGET})
    # setup_cppcheck_for_target(
    #     TARGET ${COMPONENT_TARGET}
    #     CHECKS all
    #     SUPPRESS unmatchedSuppression unusedFunction missingIncludeSystem
    # )
    # setup_clang_format_for_target(${COMPONENT_TARGET} PARENT sri-race-plugins)

    install(
        TARGETS ${COMPONENT_TARGET}
        LIBRARY DESTINATION lib/race/comms/PluginDestini
    )

    # if(ANDROID)
    #     if ("${TARGET_ARCHITECTURE}" STREQUAL "ANDROID_arm64-v8a")
    #         set(NODE_TYPE android-arm64-v8a-client)
    #     else()
    #         set(NODE_TYPE android-x86_64-client)
    #     endif()

    #     add_custom_command(TARGET ${COMPONENT_TARGET} POST_BUILD
    #         COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini
    #         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${COMPONENT_TARGET}> ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/
    #         BYPRODUCTS ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/lib${COMPONENT_TARGET}.so
    #     )
    # else()
        if ("${TARGET_ARCHITECTURE}" STREQUAL "LINUX_arm64-v8a")
            set(NODE_TYPE linux-arm64-v8a)
        else()
            set(NODE_TYPE linux-x86_64)
        endif()

        add_custom_command(TARGET ${COMPONENT_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini
            COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${COMPONENT_TARGET}> ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${COMPONENT_TARGET}> ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/
            BYPRODUCTS
                ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/lib${COMPONENT_TARGET}.so
                ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/lib${COMPONENT_TARGET}.so
        )
    # endif()

endfunction()

################################################################################
# Plugin manifest
################################################################################

if(ANDROID)
    if ("${TARGET_ARCHITECTURE}" STREQUAL "ANDROID_arm64-v8a")
        set(NODE_TYPE android-arm64-v8a-client)
    else()
        set(NODE_TYPE android-x86_64-client)
    endif()

    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json
        OUTPUT ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/manifest.json
    )
    add_custom_target(PluginDestiniManifest ALL
        DEPENDS ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/manifest.json
    )
else()
    if ("${TARGET_ARCHITECTURE}" STREQUAL "LINUX_arm64-v8a")
        set(NODE_TYPE linux-arm64-v8a)
    else()
        set(NODE_TYPE linux-x86_64)
    endif()
    
    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/
        # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mjpg_simplex/manifest.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/
        # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mjpg_simplex/manifest.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json
        OUTPUT
            ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/manifest.json
            ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/manifest.json
    )
    add_custom_target(PluginDestiniManifest ALL DEPENDS
        ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/manifest.json
        ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/manifest.json
    )
endif()

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/manifest.json
        DESTINATION lib/race/comms/PluginDestini
)

################################################################################
# Components
################################################################################

add_subdirectory(encoding)
add_subdirectory(flickr_transport)
add_subdirectory(mjpg_transport)
add_subdirectory(mjpg_simplex)
add_subdirectory(video_encoding)
