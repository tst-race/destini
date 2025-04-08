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
