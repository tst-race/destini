################################################################################
# Encoding component
################################################################################

setup_component_target(
    TARGET PluginTA2SRI_VideoEncoding
    SOURCES
        PluginTA2SRI_VideoEncoding.cpp
        CLICodec.cpp
        StringUtility.cpp
        ../common/log.cpp
)
