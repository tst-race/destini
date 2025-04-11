#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Start internal (running in a RACE node) services
#
# Note: For Two Six TA2 Plugin, there are no internal requirements. will print
# dummy config as an example and to test mounted artifacts
#
# Arguments:
# -h, --help
#     Print help and exit
#
# Example Call:
#    bash start_internal_services.sh \
#        {--help}
# -----------------------------------------------------------------------------


###
# Helper functions
###


# Load Helper Functions
BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)



###
# Main Execution
###




if [ -f /tmp/flickr_init ]; then
    exit 0
fi

#    chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/*
#    cp /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/* /usr/local/bin/
#    cd /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/scripts/
#    chmod a+x *

touch /tmp/flickr_init
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIFlickr/bin/*
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIFlickr/scripts/*


mkdir -p /ramfs/sriFlickr

cp /usr/local/lib/race/ta2/PluginTA2SRIFlickr/covers/flickr.tar /ramfs/sriFlickr
cd /ramfs/sriFlickr
tar -xf flickr.tar
find flickr -type d -exec chmod o+rx {} \;
find flickr -type f -exec chmod o+r {} \;

