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




if [ -f /tmp/imgur_init ]; then
    exit 0
fi

#    chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/*
#    cp /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/* /usr/local/bin/
#    cd /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/scripts/
#    chmod a+x *

touch /tmp/imgur_init
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIImgur/bin/*
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIImgur/scripts/*

mkdir -p /ramfs/sriImgur

cp /usr/local/lib/race/ta2/PluginTA2SRIImgur/covers/imgur.tar /ramfs/sriImgur
cd /ramfs/sriImgur
tar -xf imgur.tar
find imgur -type d -exec chmod o+rx {} \;
find imgur -type f -exec chmod o+r {} \;

