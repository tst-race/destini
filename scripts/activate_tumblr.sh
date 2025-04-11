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




if [ -f /tmp/tumblr_init ]; then
    exit 0
fi


touch /tmp/tumblr_init
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRITumblr/bin/*
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRITumblr/scripts/*


mkdir -p /ramfs/sriTumblr

cp /usr/local/lib/race/ta2/PluginTA2SRITumblr/covers/tumblr.tar /ramfs/sriTumblr
cd /ramfs/sriTumblr
tar -xf tumblr.tar
find tumblr -type d -exec chmod o+rx {} \;
find tumblr -type f -exec chmod o+r {} \;

