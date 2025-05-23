# SRI RaceBoat Plugin:  JPEG Steganographic Encoding

This plugin invokes SRI's JPEG embedding infrastructure to implement
encoding and decoding.  A message to be sent is embedded into a
randomly-selected image taken from a repository of images found in

    /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/covers/

Parameters used for encoding and decoding are specified in the
'codec.json' file in this directory.  After encoding, the result
(taking the form of a media file) is sent to a compatible transport.
The transport usually posts the result to a social media site.

For decoding, the plugin receives a byte sequence that is assumed to
contain a JPEG image (starting with "JFIF").  The plugin then uses the
parameters in 'codec.json' to decode using unwedge (part of SRI's VEIL
library suite).

## Codec Parameters

Plugins are assumed to be running in a Docker container, so most
global files can be found under /etc/race/plugins in the container.
The 'codec.json' file appears in the corresponding plugin
subdirectory, and looks like this:

```
{
    "android_path": "/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/scripts/jel2_Pixelfed_codec_android.sh",
    "path": "/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/scripts/jel2_decomposed_codec_p3.sh",

    "args": {
        "common": "-seed <secret> -bpf 1 -nfreqs 1 -maxfreqs 6",
        "encode": "<coverfile>"
    },

    "media": {
        "capacities": "/etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/sri-ta2/covers/jpeg/capacities.txt",
        "maximum": 3000
    },

    "initCommand": "/bin/bash -c /etc/race/plugins/unix/x86_64/PluginTA2SRIDecomposed/scripts/initialize_jel2.sh",

    ".format": "Decomposed-TA2",
    "mime-type": "image/jpeg",
    "encodingTime": 0.5
}
```

This file contains references to scripts that handle the process of
embedding or extracting steganographic content into / out of media.
The <seed> token represents a random shared secret seed to be used for
the pseudo-random number generator.  This value is chosen
automatically by the plugin.  Likewise, the <coverfile> token
represents a randomly chosen cover image.

## Parameters

The libjel API allows many properties to be set for dictating the
behavior of the embedding / extraction process.  The most significant
of these can be set using command line switches for the wedge, unwedge
and wcap utilities.  These parameters affect the capacity and
underlying distributions of pixel values in the steganographic result
as compared to the original cover.  Most parameters are common between
the JPEG and Video utilities (video_wedge, video_unwedge, video_wcap).

Recall that JPEG images are divided into 8x8 blocks called Minimum
Coding Units (MCUs).  Each block is represented in the frequency
domain by 64 coefficients.  The parameters described below control the
frequencies to be used and the spatial density of the embedding:

	seed <N>

This switch sets the seed <N> for the random number generated used by
'wedge' and 'unwedge'.  This affects frequency and MCU selection.

	bpf <N>

This is the bits-per-frequency switch. <N> is the number of bits to be
used for encoding in a given frequency component.

	nfreqs <N>

This switch controls the number of frequency components to be used for
embedding within each MCU.  For example, using "-bpf 1 -nfreqs 4"
means that embedding will use 4 frequencies and only encode 1 bit in
each frequency.  This means that each MCU will hold 4 bits of
steganographic information.

	mcudensity <N>

This switch represents how "full" the embedding is, i.e., what
percentage of the MCUs should be used for embedding.  If "-seed" is
provided, MCUs are selected at random.  Thus, specifying a sparse
mcudensity, like "-mcudensity 10", along with a seed, will cause the
embedding to be distributed sparsely and randomly throughout the
image.  For normal resolution images (a few K per side), this renders
the embedding virtually invisible, difficult to detect and difficult
to decode since MCU density, seed, bits per frequency, number of
frequencies, and JPEG quality factor all need to be guessed correctly
to extract the message.
