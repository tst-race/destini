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



if [ -f /tmp/avideo_init ]; then
    exit 0
fi

chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIAvideo/bin/*
chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIAvideo/scripts/*
touch /tmp/avideo_init

mkdir -p /ramfs/sriAvideo
cd /ramfs/sriAvideo
ln -s /aux_data/PluginTA2SRIAvideo/sri-ta2/covers covers
ln -s /aux_data/PluginTA2SRIAvideo/sri-ta2/.cover .cover
find . -type d -exec chmod o+rx {} \;
find . -type f -exec chmod o+r {} \;


