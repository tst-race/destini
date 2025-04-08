#!/bin/bash
#
# This script MUST take care of all installation needs for
# sri-race-plugins, and should be run within a docker container in thw
# working directory 'sri-race-plugins'
#
# bash install.sh
#
fromdir="build/LINUX_x86_64/source"
todir="/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed"

# Step 1: ensure that libraceSdkCommon.so is available to everyone,
# and that race-cli is available.  These may need to be built, but
# once present, we should be able to copy:
#

## PLV: these should be unnecessary in a raceboat-runtime container
cp /code/race-core/build/LINUX_x86_64/racesdk/common/source/libraceSdkCommon.so /usr/local/lib/
cp /code/private-race-comm-lib/build/LINUX_x86_64/app/race-cli/race-cli /usr/local/bin/

## PLV: _must_ these be in the executable path or can they be plugin/raceboat relative?
cp /code/jel2/wedge /code/jel2/unwedge /code/jel2/wcap /code/jel2/jquant /usr/local/bin/
#
#
# Step 2: Copy .so files for each plugin into the appropriate
# /etc/race/plugins subdirectory:
#

## PLV: these should just be mounted with the kits? These _are_ the kits?
cp $fromdir/encoding/libPluginTA2SRICLIEncoding.so               $todir/libPluginTA2SRICLIEncoding.so
cp $fromdir/jel_encoding/libPluginTA2SRI_JELEncoding.so          $todir/libPluginTA2SRI_JELEncoding.so
cp $fromdir/video_encoding/libPluginTA2SRI_VideoEncoding.so      $todir/libPluginTA2SRI_VideoEncoding.so
cp $fromdir/flickr_transport/libPluginTA2SRIFlickrTransport.so   $todir/libPluginTA2SRIFlickrTransport.so
#
#
# Step 3: Copy the bash scripts that perform low-level codec operations:
#

## PLV: Are these calling wedge/unwedge/wcap/jquant above? Can they be modified to call a non-global path
cp scripts/jel/*.sh      $todir/scripts/
cp scripts/video/*.sh     $todir/scripts/

cp /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/bin/* /usr/local/bin/
#
#
# Step 4: Copy the codec descriptors (json) to the appropriate channel
# subdirectories:
#
## PLV: Why are these in top-level /etc/race/etc/? Can these live with the kit executables?
mkdir -p /etc/race/etc/jelFlickr /etc/race/etc/videoFlickr
cp source/jel_encoding/jel_codec.json          /etc/race/etc/jelFlickr
cp source/video_encoding/video_codec.json        /etc/race/etc/videoFlickr
#
#
# Step 5:  Copy data files for authentication and wordlists:
#
## PLV: These should be changed to parameters and/or stored with the kits
cp -r source/flickr_transport/flickr_auth /etc/race/
cp source/flickr_transport/wordlist.txt /etc/race/
#
# What about manifests?  Need to organize and copy them as well.
## PLV: Manifests should live with kits

## PLV: top-level comment:
## This whole script should not exist, everything should just exist in the kits directory and the plugin code should be modified accordingly to use that
