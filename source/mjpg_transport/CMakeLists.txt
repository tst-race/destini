################################################################################
# Transport component
################################################################################

# This is ok for the new race-core:

setup_component_target(
    TARGET PluginTA2SRI_MJPG_Transport
    SOURCES
        MJPGLink.cpp
        LinkAddress.cpp
        LinkMap.cpp
        MessageHashQueue.cpp
        PluginTa2SRI_MJPG_Transport.cpp
	mjpg_transport.c
	../common/log.cpp
)

