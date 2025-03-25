#!/bin/bash

######
# This script must be run in an sri-plugin Docker container.  See
# Dockerfile in this same directory.  That container will have most of
# the packages installed:
#####

# In the latest version, we will mount the plugin sandbox with
# modified manifests, so you do NOT need to copy them:

#set p = PluginTA2SRIDecomposed
#cp my_manifest2.json /etc/race/plugins/unix/x86_64/$p/

# Run docker as follows:

# docker run -it -v `pwd`:/code/ -v /tmp/:/tmp/ -v /opt/projects/RACE/twosix/plugins/unix/x86_64/:/etc/race/plugins/unix/x86_64/ --network=rib-overlay-network ghcr.io/tst-race/private-race-core/race-sdk:main bash

#mkdir -p /usr/local/lib/race/ta2/
#ln -s /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed /usr/local/lib/race/ta2/

# Stage libflickcurl.so:
#cp /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/libflickcurl.so /usr/local/lib/libflickcurl.so
#cp /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/libflickcurl.so /usr/local/lib/race/ta2/libflickcurl.so

# Stage codec.json:
# cp /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/codec.json /etc/race/plugins/
# cp /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/codec.json /usr/local/lib/race/ta2/PluginTA2SRIDecomposed/

# Stage SRI plugin .so files:
cp /code/sri-race-plugins/build/LINUX_x86_64/source/jel_encoding/libPluginTA2SRICLIEncoding.so /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/
cp /code/sri-race-plugins/build/LINUX_x86_64/source/flickr_transport/libPluginTA2SRIFlickrTransport.so /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/
cp /code/sri-race-plugins/build/LINUX_x86_64/source/mjpg_transport/libPluginTA2SRI_MJPG_Transport.so /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/

#apt-get update

#apt-get -y install libthrift-dev
#apt-get -y install libjpeg62-dev
#apt-get -y install libjsoncpp-dev
#apt-get -y install libevent-dev

#apt-get -y install libboost-all-dev

#apt-get -y install libcpprest-dev
#apt-get -y install libxml2-dev

pushd /usr/local/include
ls /usr/include
ln -s /usr/include/libxml2/libxml
popd

# Already built by sri-race-plugins cmake framework:
pushd /code/sri-race-plugins/3rd-party/flickcurl-1.26
make install
popd

pushd /code/jel2/py
python setup.py install
popd


#
# Stage the twosix stuff:
#

cp /code/private-race-comm-lib/build/LINUX_x86_64/app/race-cli/race-cli /usr/local/bin/
ln -s /code/private-race-comm-lib/build/LINUX_x86_64/source/librace-comm-lib.so /usr/local/lib/librace-comm-lib.so
ldconfig /usr/local/lib

cp -r /code/sri-race-plugins/source/flickr_transport/flickr_auth /etc/race/

cp /code/race-core/build/LINUX_x86_64/racesdk/common/source/libraceSdkCommon.so /usr/local/lib
cp /code/race-core/build/LINUX_x86_64/racesdk/core/source/libraceSdkCore.so /usr/local/lib

cp -r /code/sri-race-plugins/mjpg_streamer/streamer /root/

# For now, we need to build the flickr transport within the race-core
# build framework.  Otherwise nlohmann/json.hpp will throw compile
# errors.  Not sure why.  Once built, you can copy:

# cp /code/race-core/build/LINUX_x86_64/plugin-comms-twosix-decomposed-cpp/source/flickr_transport/libPluginTA2SRIFlickrTransport.so /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/
