# SRI Flickr Transport for RaceBoat

This directory contains the Flickr transport plugin for RaceBoat.
This plugin will post and retrieve media from Flickr.  Images and
videos are assumed to have steganographic content embedded using SRI's
VEIL system (video and image steganography).  This plugin accepts and
generates byte blobs that are steganographic media.  In the case of
sending (posting) the plugin assumes that encoding has already been
performed.  In the case of retrieval, the plugin downloads media and
returns the resulting byte blobs to be decoded by an encoder plugin.


## Authentication

The core Flickr API is implemented using the 'flickcurl' library.
This requires authentication, which is supplied by the files in the
flickr_auth subdirectory.  Legal users are 'user1' through 'user6'.
These names are used to find the '.ini' files that flickcurl uses for
authentication.  These files are staged when a sri-plugin Docker
container is initialized and can be found under the '/etc/race'
directory in the running Docker container.


## Channels

The following 'manifest.json' file provides an example of composition
using the JPEG steganographic encoder along with the Flickr transport.
Specifically, note that the "plugins" section contains the selections
of transport and encoding that allow us to combine steganographic
embedding and extraction with Flickr posting and downloading,
respectively (for the full description of parameters, please see the
corresponding RaceBoat documentation for the manifest format and
semantics):

```
{
    "channel_properties" : {
	"jelTwoSixIndirect": {
	    "bootstrap": false,
	    "channelGid": "jelTwoSixIndirect",
	    "connectionType": "CT_INDIRECT",
	    "creatorExpected": {
		"send": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		},
		"receive": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		}
	    },
	    "description": "Implementation of the Two Six Labs Indirect communications utilizing the Two Six Whiteboard",
	    "duration_s": -1,
	    "linkDirection": "LD_BIDI",
	    "loaderExpected": {
		"send": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		},
		"receive": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		}
	    },
	    "mtu": -1,
	    "multiAddressable": false,
	    "period_s": -1,
	    "reliable": false,
	    "isFlushable": false,
	    "sendType": "ST_STORED_ASYNC",
	    "supported_hints": ["polling_interval_ms", "after"],
	    "transmissionType": "TT_MULTICAST",
	    "maxLinks": 1000,
	    "creatorsPerLoader": -1,
	    "loadersPerCreator": -1,
	    "roles": [
		{
		    "roleName": "default",
		    "mechanicalTags": [],
		    "behavioralTags": [],
		    "linkSide": "LS_BOTH"
		}
	    ],
	    "maxSendsPerInterval": -1,
	    "secondsPerInterval": -1,
	    "intervalEndTime": 0,
	    "sendsRemainingInInterval": -1
	},
	"jelFlickr": {
	    "bootstrap": false,
	    "channelGid": "jelFlickr",
	    "connectionType": "CT_INDIRECT",
	    "creatorExpected": {
		"send": {
		    "bandwidth_bps": 10000,
		    "latency_ms": 7000,
		    "loss": 0.1
		},
		"receive": {
		    "bandwidth_bps": 10000,
		    "latency_ms": 7000,
		    "loss": 0.1
		}
	    },
	    "description": "Implementation of SRI's JPEG steg. encoder utilizing a Flickr transport",
	    "duration_s": -1,
	    "linkDirection": "LD_LOADER_TO_CREATOR",
	    "loaderExpected": {
		"send": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		},
		"receive": {
		    "bandwidth_bps": 277200,
		    "latency_ms": 3190,
		    "loss": 0.1
		}
	    },
	    "mtu": 3200,
	    "multiAddressable": false,
	    "period_s": -1,
	    "reliable": false,
	    "isFlushable": false,
	    "sendType": "ST_STORED_ASYNC",
	    "supported_hints": [],
	    "transmissionType": "TT_UNICAST",
	    "maxLinks": 1000,
	    "creatorsPerLoader": -1,
	    "loadersPerCreator": -1,
	    "roles": [
		{
		    "roleName": "default",
		    "mechanicalTags": [],
		    "behavioralTags": [],
		    "linkSide": "LS_BOTH"
		}
	    ],
	    "maxSendsPerInterval": -1,
	    "secondsPerInterval": -1,
	    "intervalEndTime": 0,
	    "sendsRemainingInInterval": -1
	}	
    },
    "channel_parameters" : [
    ],
    "plugins": [
	{
            "file_path": "PluginTA2SRIDecomposed",
            "plugin_type": "TA2",
            "file_type": "shared_library",
            "node_type": "any",
            "shared_library_path": "libPluginTA2SRIFlickrTransport.so",
            "channels": [],
            "transports": ["flickrDecomposed"],
            "usermodels": [],
            "encodings": []
	},
        {
            "file_path": "PluginTA2SRIDecomposed",
            "plugin_type": "TA2",
            "file_type": "shared_library",
            "node_type": "any",
            "shared_library_path": "libPluginTA2SRICLIEncoding.so",
            "channels": [],
            "transports": [],
            "usermodels": [],
            "encodings": ["SRICLIEncoding"]
        }	
    ],
    "compositions": [
        {
            "id": "jelTwoSixIndirect",
            "transport": "twoSixIndirect",
            "usermodel": "twoSixChrisTodd",
            "encodings": ["SRICLIEncoding"]
        },
        {
            "id": "jelFlickr",
            "transport": "flickrDecomposed",
            "usermodel": "twoSixChrisTodd",
            "encodings": ["SRICLIEncoding"]
        }	
    ]
}
```






