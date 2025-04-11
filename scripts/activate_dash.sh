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


function _dash_init()
{

    if [ -f /tmp/dash_init ]; then
	exit 0
    fi

    chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIDash/bin/*
    chmod a+x /usr/local/lib/race/ta2/PluginTA2SRIDash/scripts/*
    touch /tmp/dash_init
    
    mkdir -p /ramfs/sriDash
    cd /ramfs

    ln -s /aux_data/PluginTA2SRIDash/sri-ta2/covers /ramfs/sriDash/covers
    ln -s /aux_data/PluginTA2SRIDash/sri-ta2/.cover /ramfs/sriDash/.cover

    mkdir -p /ramfs/sriDash/steg/dash
    mkdir -p /ramfs/sriDash/steg/jpeg

    cat /ramfs/sriDash/covers/jpeg/capacities.txt | sed -e 's/\-ta2/Dash/g' > /ramfs/sriDash/steg/jpeg/capacities.txt

    find sriDash -type d -exec chmod o+rx {} \;
    find sriDash -type f -exec chmod o+r {} \;

}

function _nginx_activate()
{
    nginx_cnt=`pgrep nginx |wc -l`
    sed -i 's/sri\-ta2/sriDash/g' /usr/local/nginx/conf/nginx.conf
    
    if [ $nginx_cnt == 0 ]; then
	cd /usr/local/lib/race/ta2/PluginTA2SRIDash/scripts
	systemctl start nginx.service
    fi
    
}

case $1 in
    init)
	_dash_init
	;;
    activate)
	_nginx_activate
	;;
esac
      
