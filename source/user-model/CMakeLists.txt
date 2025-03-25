################################################################################
# User Model component
################################################################################

setup_component_target(
    TARGET PluginTa2TwoSixStubUserModel
    SOURCES
        LinkUserModel.cpp
        MarkovModel.cpp
        PluginTa2TwoSixStubUserModel.cpp
        ../common/log.cpp
)
