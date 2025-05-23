################################################################################
# Transport component
################################################################################

# This is ok for the new race-core:

setup_component_target(
    TARGET PluginTA2SRIFlickrTransport
    SOURCES
        FlickrLink.cpp
	flickr_transport.c
        DynamicWords.cpp
        LinkAddress.cpp
        LinkMap.cpp
        MessageHashQueue.cpp
        PluginTa2SRIFlickrTransport.cpp
	../common/log.cpp
)

