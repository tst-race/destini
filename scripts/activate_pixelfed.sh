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




if [ -f /tmp/pixelfed_init ]; then
    exit 0
fi

#    chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/*
#    cp /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/* /usr/local/bin/
#    cd /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/scripts/
#    chmod a+x *

touch /tmp/pixelfed_init
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/bin/*
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/scripts/*


mkdir -p /ramfs/sriPixelfed

cp /usr/local/lib/race/ta2/PluginTA2SRIPixelfed/covers/sri-ta2.tar /ramfs/sriPixelfed
cd /ramfs/sriPixelfed
tar -xf sri-ta2.tar
find sri-ta2 -type d -exec chmod o+rx {} \;
find sri-ta2 -type f -exec chmod o+r {} \;

