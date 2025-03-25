# SRI Motion JPEG Video Transport for RaceBoat

This directory contains a DUPLEX MJPG video transport plugin for
RaceBoat.  This plugin sets up two motion JPEG feeds and supports
steganographic insertion within the feeds.  Motion JPEG videos can
have steganographic content embedded using SRI's VEIL system (video
and image steganography).  Receivers use the JEL checksum to determine
if there is any content in a given frame.  Invalid checksums cause the
frame to be ignored.


This plugin follows the same basic structure as the Flickr transport,
except that instead of post and get, the plugin "posts" to the
sender's motion jpeg frames and "get"s from the receiver's motion jpeg
client stream.


## Streamer

No authentication is needed, but streaming Motion JPEG servers must be
started on each side of the link.  This is done automatically by the
transport.  The streamer should be installed in /root/streamer in the
Docker container that you are running in.  The necessary files for the
streamer and client can be found in:

	 sri-race-plugins/mjpg_streamer/streamer

These files are normally copied to '/root' when
'sri-race-plugins/stage.sh' is invoked.

## Testing

Here we are assuming that the root directory containing RACE code is at "/opt/projects/RACE/twosix".
For MJPG (motion jpeg), start two docker containers to test each side, for example:

    docker run -it --rm -v /opt/projects/RACE/twosix:/code -v /opt/projects/RACE/twosix/etc_plugins:/etc/race/plugins -p 8080:8080 -p 8081:8081 sri-plugin-streamer:latest

Then for send in the first docker container, run this:

     race-cli  --send  --send-channel=jelMJPG  --send-address='{"hashtag":"server-to-client","hostname":"twosix-whiteboard","port":8080}'  --debug

and for receive in the other docker container, run this:

    race-cli  --recv  --recv-channel=jelMJPG  --send-address='{"hashtag":"server-to-client","hostname":"twosix-whiteboard","port":8081}'  --debug


## Channels

The following 'manifest.json' file provides an example of composition
using the streaming Motion JPEG transport.  Specifically, note that
the "plugins" section contains the selections of transport and
encoding that allow us to combine steganographic embedding and
extraction with insertion and extraction into/from the motion JPEG
video streams (for the full description of parameters, please see the
corresponding RaceBoat documentation for a description of the manifest
format):

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
	"jelMJPG": {
	    "bootstrap": false,
	    "channelGid": "jelMJPG",
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
	    "description": "Implementation of SRI's streaming mjpg communications",
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
            "shared_library_path": "libPluginTA2SRI_VideoEncoding.so",
            "channels": [],
            "transports": [],
            "usermodels": [],
            "encodings": ["SRIVideoEncoding"]
        },
        {
            "file_path": "PluginTA2SRIDecomposed",
            "plugin_type": "TA2",
            "file_type": "shared_library",
            "node_type": "any",
            "shared_library_path": "libPluginTA2SRI_MJPG_Transport.so",
            "channels": [],
            "transports": ["MJPGDecomposed"],
            "usermodels": [],
            "encodings": []
        }	
    ],
    "compositions": [
        {
            "id": "jelTwoSixIndirect",
            "transport": "twoSixIndirect",
            "usermodel": "twoSixChrisTodd",
            "encodings": ["SRIVideoEncoding"]
        },
        {
            "id": "jelMJPG",
            "transport": "MJPGDecomposed",
            "usermodel": "twoSixChrisTodd",
            "encodings": ["noop"]
        }	
    ]
}
```