#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Stop internal (running in a RACE node) services
#
# Arguments:
# -h, --help
#     Print help and exit
#
# Example Call:
#    bash deactivate_minecraft.sh \
#        {--help}
# -----------------------------------------------------------------------------


##
# Helper function
##

_killForArg()	# <process search string>
{
    _pid=$(ps auxwwww | grep -E "$1" | grep -v grep | awk '{print $2}')
    if [ -n "$_pid" ]; then
	kill -9 $_pid
    else
	echo "Process containing \"$1\" not found."
    fi
}

###
# Arguments
###

# Parse CLI Arguments
while [ $# -gt 0 ]
do
    key="$1"

    case $key in
        -h|--help)
        echo "Example Call: bash $(basename "$0")"
        exit 1;
        ;;
        *)
        echo "ERROR: unknown argument \"$1\""
        exit 1
        ;;
    esac
done


###
# Main Execution
###

# IP or hostname?
# doesn't have to match config
export PERSONA=$(hostname) # hostname | nslookup

export MC_SVR_IP=$(echo "$PERSONA" | sed -E 's/.$/1/')      # cons'd as "first" server host

# Kill socks proxy

_killForArg 'java SOCKS.SOCKSProxy'

if [ $PERSONA = $MC_SVR_IP ]; then

    _killForArg 'forge-1.15.2-31.2.57-no_template_multiclient.jar nogui'
    _killForArg 'FlaskMaildrop.py'

else

    _killForArg 'FlaskMCTA2.py'
    _killForArg 'Dminecraft.client.jar'
    _killForArg 'Xvfb'

fi

