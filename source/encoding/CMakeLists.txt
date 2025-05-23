################################################################################
# Encoding component
################################################################################

setup_component_target(
    TARGET PluginTA2SRICLIEncoding
    SOURCES
        PluginTA2SRICLIEncoding.cpp
        CLICodec.cpp
        StringUtility.cpp
        ../common/log.cpp
)

if(ANDROID)
    if ("${TARGET_ARCHITECTURE}" STREQUAL "ANDROID_arm64-v8a")
        set(NODE_TYPE android-arm64-v8a-client)
    else()
        set(NODE_TYPE android-x86_64-client)
    endif()

    add_custom_command(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/codec.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/codec.json
        OUTPUT ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/codec.json
    )
    add_custom_target(PluginCLIEncodingCodecJson ALL
        DEPENDS ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}/PluginDestini/codec.json
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
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/codec.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/codec.json ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/codec.json
        OUTPUT
            ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/codec.json
            ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/codec.json
    )
    add_custom_target(PluginCLIEncodingCodecJson ALL DEPENDS
        ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-client/PluginDestini/codec.json
        ${PLUGIN_BASE_DIR}/artifacts/${NODE_TYPE}-server/PluginDestini/codec.json
    )
endif()
