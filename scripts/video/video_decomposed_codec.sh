#!/bin/bash

# This is the high-level mjpg codec for the SRI encoder RACE plugin.

# Requires scripts mjpeg_wedge and mjpeg_unwedge

# Requires the jel library, libjpeg including jpegoptim, and netlib
# utils jpegtopnm and ppmtojpeg.

WEDGE=video_wedge
UNWEDGE=video_unwedge
KEEPTMP=1
NOSIZECHECK=1

. $(dirname $0)/_video_codec_base.sh
