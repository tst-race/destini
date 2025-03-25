#!/usr/bin/env python

import os
import sys
import glob
import json
from subprocess import call


# SWIL testing for Flickr: Using favored parameters, this script stegs
# a message in video, then uses the standalone flickr_transport to
# upload as one user and download as a different user, finally
# unwedging the message from the video.  The goal is to determine
# whether a cover and associated parameters resulted in clean message
# transmission.

# This stinks:

plugin_root = "../../"


transport_prog = os.path.join(plugin_root, "source/flickr_transport/ft")
video_wedge = os.path.join(plugin_root, "source/flickr_transport/video_wedge")
video_unwedge = os.path.join(plugin_root, "source/flickr_transport/video_unwedge")
video_wcap = os.path.join(plugin_root, "source/flickr_transport/video_wcap")
    
if not os.path.exists(transport_prog):
    print("WARNING: The 'ft' flicker transport executable was not found in the RACE Flickr")
    print("         transport plugin source directory.")
    print(f"         It should be at {transport_prog}")
    print("         To build it, cd to that directory and compile with gcc as follows:")
    print("         gcc -o ft -DTEST2 flickr_transport.c /usr/local/lib/libflickcurl.a -lxml2 -lcurl")

if False:
    # Just a template for swil configs:
    config = {
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
    


def read_swil_config(filename):
    with open(filename) as f:
        config = json.load(f)
    print(json.dumps(config, indent=4))
    # The config file offers the option of using different versions of
    # video_wedge:
    try:
        video_wedge = config['video_wedge']
        print(f"Using video_wedge in {video_wedge}")
    except:
        pass

    try:
        video_unwedge = config['video_unwedge']
        print(f"Using video_unwedge in {video_unwedge}")
    except:
        pass
    
    try:
        video_wcap = config['video_wcap']
        print(f"Using video_wcap in {video_wcap}")
    except:
        pass
    
    return config


    
def user_authfile(username):
    auth_file = os.path.join(plugin_root, f"source/flickr_transport/flickr_auth/{username}.ini")
    if os.path.exists(auth_file):
        return auth_file
    else:
        return None


def make_switch(s, cfg):
    return [f"-{s}", str(cfg[s])]


def wedge_command(cover, message, output, cfg):
    args = [ video_wedge ]
    common = cfg['common']
    for key in common:
        args = args + make_switch(key, common)
    wparam = cfg['wedge']
    for key in wparam:
        args = args + make_switch(key, wparam)
    args = args + [ "-cover", cover, "-message", message, "-output", output]
    return args

def unwedge_command(steg, message, cfg):
    args = [ video_unwedge ]
    common = cfg['common']
    for key in common:
        args = args + make_switch(key, common)
    uwparam = cfg['unwedge']
    for key in uwparam:
        args = args + make_switch(key, uwparam)
    args = args + [ "-steg", steg, "-message", message]
    return args



def compare_messages(msg_in, msg_out):
    with open(msg_in) as f:
        s1 = f.read()

    with open(msg_out) as f:
        s2 = f.read()

    print(f"Message in: \n {msg_in}\n")
    print(f"Message out: \n {msg_out}\n")

    if s1 == s2:
        print("Messages are identical!")
    else:
        print("Messages differ!")
          

# Args: swil <config> <cover>

# All configuration info (like transports, covers, messages, etc.)
# should appear in the <config> JSON file.

if len(sys.argv) == 1 or len(sys.argv) > 4:
    print("Usage: swil <config> [ <cover> [<msg>] ]")
else:

    config = read_swil_config(sys.argv[1])

    #####  All input (wedge) parameters:

    wedge_params = config['wedge']

    try:
        cover = wedge_params['cover']
    except:
        if len(sys.argv) < 3:
            print("You must specify a cover either in the command line or in the config file!")
            quit()
        else:
            cover = sys.argv[2]

    if len(sys.argv) >= 4:
        msg_in = sys.argv[3]
    else:
        msg_in = wedge_params['message']


    to_flickr = wedge_params['output']

    ####################################

    #####  All output (unwedge) parameters:

    unwedge_params = config['unwedge']
    from_flickr = unwedge_params['steg']
    msg_out = unwedge_params['message']

    ####################################

    print("Files used:")

    print(f"   cover:       {cover}")
    print(f"   msg_in:      {msg_in}")
    print(f"   msg_out:     {msg_out}")

    print(f"   from_flickr: {from_flickr}")
    print(f"   to_flickr:   {to_flickr}")

    wedge  = wedge_command(cover, msg_in, to_flickr, config)
    print(f"Wedge command:  {wedge}")

    # For now, all the information needed for the transport stub
    # appears in the config file under "transport" => "command"

    transport = config['transport']['command']
    print(f"Transport:  {transport}")
    
    unwedge  = unwedge_command(from_flickr, msg_out, config)
    print(f"Unwedge command:  {unwedge}")

    print("\n===Processing===\n")

    rc = call( wedge )
    print(f"call to wedge returned {rc}")
    
    rc = call( transport)
    print(f"call to transport returned {rc}")

    rc = call( unwedge )
    print(f"call to video_unwedge returned {rc}")

    compare_messages(msg_in, msg_out)

