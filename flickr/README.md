# Social Media Testing and Simulation

## Simulator Usage

This subdirectory is an umbrella for experiments with social media
sites.  The 'sim' subdirectory contains a software simulator called
'swil.py'.  This script allows you to construct a simulation of a
social media site's transcoding policies.

       Usage: "swil.py <config> [ <cover> [<msg>] ]"


The 'swil.py' script requires a JSON config file as its first
argument.  Each config file has sections for invoking wedge,
transcoding, then unwedge.  Here's an example configuration file for
Flickr:

```
{
    "#" : "Flickr simulator to be used with swil.py.",
    "#" : "This version uses constant rate factor '-crf NN' to throttle quality.",
    "#" : "Results for crf=22.5: fail  on butterfly1, butterfly2, czech, birds, pass on gekas.",
    "#" : "This is in line with Flickr's behavior.",
    "common" : {
	"ecc" : 8,
        "bpf" : 1,
        "nfreqs" : 1,
        "seed" : 1,
        "jpeg_quality" : 75
    },
    "wedge" : { 
	"message"   : "/opt/projects/RACE/twosix/sri-race-plugins/flickr/decl.txt",
	"cache_dir" : "./cache",
	"output"    : "/tmp/to_flickr.mp4"
    },
    "transport" : {
	"command" : [ "ffmpeg",
		      "-i", "/tmp/to_flickr.mp4",
		      "-y",
		      "-c:v", "libx264",
		      "-g", "15",
		      "-pix_fmt", "yuvj420p",
		      "-strict", "-2",
		      "-crf", "22.5",
		      "/tmp/from_flickr.mp4"
		    ]
    },
    "unwedge" : {
	"steg"    : "/tmp/from_flickr.mp4",
	"message" : "/tmp/from_flickr.txt"
    }
}
```

The "common" section contains parameters that are needed for both
wedge and unwedge.  Here we are using ECC, 1 bit per frequency, 1
frequency entry, a JPEG quality of 75, and a seed of 1 for the
pseudo-random number generator.

The "wedge" section specifies the message file, the cache directory,
and an output steg video file.

The "transport" section supplies the name of an executable along with
desired command line switches.  This section can be used to upload /
download video to / from a social media site, or to simulate that
site's transcoding policy to help predict whether steganographic
content will survive the round trip.  In the case above, simulation is
performed using ffmpeg and a list of parameters that seem to mimic
Flickr's transcoding policy.  This section must specify input and
output files that can be referenced by the "wedge" and "unwedge"
sections.

The "unwedge" section specifies non-common arguments to the
video_unwedge script.  Here, we specify the steg video as produced by
the "transport" section, along with the file to be used for the
extracted message.

Success or failure can be determined by comparing the "message" file
specified in the "wedge" section with the "message" file in the
"unwedge" section.

## Authentication

The 'ft' program in particular accesses Flickr for testing, and
provides "gold standard" steg. survival results for building Flickr
simulations.  Authentication information for 'ft' is stored in '.ini'
files that are stored in the directory
"source/flickr_transport/flickr_auth".  Authentication files for 'ft'
end in '.ini'.


## Parameter Selection

The ffmpeg transcoder is used to simulate social media sites.  The
most critical parameters are "-crf", followed by "-g".  The latter sets
the interval for I-frames.  A setting of 15 for I-frame intervals is
very common, and deviations from this value might be considered a tell
of unusual post-processing.

The "-crf" (Constant Rate Factor) parameter tries to maintain a
constant bit rate for the resulting video.  We use this parameter to
mimic the effect of a whiteboard on video.  Higher values of the
constant rate factor represent lower bit rates.  Here, a value of 22.5
is used to simulate Flickr's preferred bit rate.

To determine the proper CRF value, we selected a set of test videos
(in this case, 5 were used), and pumped these videos through Flickr to
generate Flickr-friendly cover videos.  We then encoded content into
each of these to test survival or failure through Flickr.  We then
replicated the survival / failure rates with ffmpeg by taking the
failed videos and incrementing the "-crf" value starting low (~12) and
progressing upward until failure occurred.  Since "-crf" is a floating
point value, it is possible to use smaller steps to obtain a threshold
"-crf" value that mimics Flickr's behavior on these test videos.
Further refinement can be done by using bisection between success and
failure values of "-crf".


## Pumping Through Flickr

The simulator can also be used to pump content through a site.  Here
is an example of a configuration file that uses the 'ft' executable to
push videos through:

```{
    "common" : {
        "bpf" : 1,
        "nfreqs" : 1,
        "seed" : 1,
        "jpeg_quality" : 75
    },
    "wedge" : { 
	"message"   : "/opt/projects/RACE/twosix/flickr/decl.txt",
	"cache_dir" : "./cache",
	"output"    : "/tmp/to_flickr.mp4"
    },
    "unwedge" : {
	"steg"    : "/tmp/from_flickr.mp4",
	"message" : "/tmp/from_flickr.txt"
    },
    "transport" : {
	"command" : [ "../../sri-race-plugins/source/flickr_transport/ft",
                      "../../sri-race-plugins/source/flickr_transport/flickr_auth/user3.ini",
		      "/tmp/to_flickr.mp4",
                      "../../sri-race-plugins/source/flickr_transport/flickr_auth/user4.ini",
		      "/tmp/from_flickr.mp4"
		    ]
    }
}
```

The 'ft' program is linked and compiled using the flickcurl C API
along with the flickr_transport.c code found in
source/flickr_transport.  See flickr_transport.c for the gcc command
used to generate 'ft'.  The 'ft' program takes four arguments:

     Usage: ft <user1> <video_in> <user2> <video_out>

where "user1" and "user2" are authentication files for Flickr (ending
in ".ini") while "<video_in>" and "<video_out>" are the uploaded and
downloaded videos respectively.  The 'ft' program will upload and post
"<video_in>" while logged in to Flickr as "user1", then download the
posted video logged in as user "user2".
