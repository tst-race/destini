# Destini

Destini is a set of Encoding and Transport component plugins for use with the [Raceboat](https://github.com/tst-race/raceboat) framework. This repo includes code for:

* An Encoding component (SRIVideoEncoding) that steganographically encodes data into MJPG files

* An Encoding component (SRICLIEncoding) that steganographically encodes data into JPEG files

## Current Status

Only the image encoding, SRICLIEncoding, has been verified to work with the current Raceboat framework, using the [decomposed exemplar](https://github.com/tst-race/decomposed-exemplars) redis-based Transport (twoSixIndirect) and user models (rapidUser) as the other components.

## Building

Destini can be built for x86-64 architectures via a one-step dockerized build script.

```
git clone https://github.com/tst-race/destini.git

cd destini

./build_artifacts_in_docker_image.sh

```

This will take some time because while the build uses the raceboat-plugin-builder image, it additionally installs several development dependencies before actually building the Destini code. The script will output a `kit/linux-x86_64-server/PluginDestini` directory. This directory contains the plugins, manifest, and necessary local scripts, executables, and data for running Destini.

## Using

Copy the `PluginDestini` directory to a directory structure alongside `PluginCommsTwoSixStubDecomposed` (from [decomposed-exemplars](https://github.com/tst-race/decomposed-exemplars) since those plugins will be necessary for providing the Transport and User Model.

Specify the channel as `jelTwoSixIndirect` to use a composed channel consisting othe `twoSixIndirect` transport, which uses redis, `rapidUser`, which provides a high activity model of post and fetch actions, and `SRICLIEncoding` which steganographically encodes data into images that are then posted through redis.

## Known Issues

The `jelTwoSixIndirect` composition appears to truncate messages longer than 4094B to 4094B. 


