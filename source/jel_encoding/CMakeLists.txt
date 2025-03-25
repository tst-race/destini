################################################################################
# Encoding component
################################################################################

setup_component_target(
    TARGET PluginTA2SRI_JELEncoding
    SOURCES
        PluginTA2SRI_JELEncoding.cpp
        CLICodec.cpp
        StringUtility.cpp
        ../common/log.cpp
        PluginTa2TwoSixBase64Encoding.cpp
        PluginTa2TwoSixNoopEncoding.cpp
        PluginTa2TwoSixStubEncoding.cpp
)
