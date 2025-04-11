#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Start internal (running in a RACE node) services
#
# Arguments:
# -h, --help
#     Print help and exit
#
# Example Call:
#    bash activate_minecraft.sh \
#        {--help}
# -----------------------------------------------------------------------------


###
# Helper functions
###

# Load Helper Functions
BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)


###
# Arguments
###

# Parse CLI Arguments
while [ -n "$1" ]; do
    key="$1"; shift

    case $key in
        -h|--help)
        echo "Example Call: bash start_internal_services.sh"
        exit 1;
        ;;
        *)
        echo "ERROR: unknown argument \"$1\""
        exit 1
        ;;
    esac
done

###
# Global
###
_PROXY_SENTINEL_FILE='/tmp/proxy_started.log'

rm -f /log/flaskmc.out


###
# Main Execution
###

cd ${BASE_DIR}

if [ ! -d /Minecraft-cached ]; then
    #    cp -r /aux_data/PluginTA2SRIMinecraft/Minecraft-cached /usr/local/lib/race/ta2/PluginTA2SRIMinecraft/Minecraft-cached
    cp -r /aux_data/PluginTA2SRIMinecraft/Minecraft-cached /Minecraft-cached
fi

PROXY_JAVA=/usr/lib/jvm/java-8-openjdk-amd64/bin
export PATH=${JAVA_HOME}/bin:${PATH}

# IP or hostname?
# doesn't have to match config
export PERSONA=$(hostname) # hostname | nslookup

# export MC_PROXY_DIR=/usr/local/lib/race/ta2/PluginTA2SRIMinecraft/MinecraftProxy
export MC_SVR_IP=$(echo "$PERSONA" | sed -E 's/.$/1/')      # cons'd as "first" server host
export MD_HOST=$MC_SVR_IP #(must be resolvable name)
export MC_PROXY_PORT=11011 #(must be resolvable name)
export NO_MC_MEMCK=1

rm -f "$_PROXY_SENTINEL_FILE"

if [ $PERSONA = $MC_SVR_IP ]; then

    cd /Minecraft-cached/Java_NIO_SOCKS/src
    git checkout remote
    rm -f SOCKS/*.class
    $PROXY_JAVA/javac SOCKS/SOCKSProxy.java
    $PROXY_JAVA/java SOCKS.SOCKSProxy > /log/mc-server-socks.log 2>&1 &

    cd /Minecraft-cached/mc2_server
    java -Xmx1024M -Xms1024M -jar forge-1.15.2-31.2.57-no_template_multiclient.jar nogui > /log/mcserver.log 2>&1 &

    cd /usr/local/lib/race/ta2/PluginTA2SRIMinecraft/destini-mc/src
    CONFIG=/usr/local/lib/race/ta2/PluginTA2SRIMinecraft/whiteboard.json python3 FlaskMaildrop.py --host 0.0.0.0 > /log/mc-maildrop.log 2>&1 &

    while ! nc -zv 127.0.0.1 15926; do
	echo "sleeping for 10 secs"
	sleep 10
    done

    while ! nc -zv $MC_SVR_IP 25565; do
	echo "sleeping for 10 secs"
	sleep 10
    done

else

    while ! nc -zv $MC_SVR_IP 25565; do
	echo "sleeping for 10 secs"
	sleep 10
    done

    export PERSONA=$(hostname | sed -E 's/^race-//')

    cd /Minecraft-cached/Java_NIO_SOCKS/src
    git checkout local
    rm -f SOCKS/*.class
    $PROXY_JAVA/javac SOCKS/SOCKSProxy.java
    $PROXY_JAVA/java SOCKS.SOCKSProxy 11011 22022 > /log/mc-client-socks.log 2>&1 &

    cd /usr/local/lib/race/ta2/PluginTA2SRIMinecraft/destini-mc/src

    xvfb-run python3 FlaskMCTA2.py --host 0.0.0.0 --auto-start --abend \
	     --mc_launcher /usr/local/lib/race/ta2/PluginTA2SRIMinecraft/destini-mc/bash/MCClientLauncher-generic.sh \
	     --mc_client_dir /Minecraft-cached/mc2_client \
	     > /log/flaskmc.out 2>&1 &

    #sleep 90
    #curl -X POST --data-binary @/usr/local/lib/race/ta2/PluginTA2SRIMinecraft/destini-mc/src/pygui/registerClient.py http://localhost:15926/eval

    while ! nc -zv 127.0.0.1 11011; do
	echo "sleeping for 10 secs"
	sleep 10
    done

    while [ $(grep "register with the MC server" /log/flaskmc.out |wc -l) -lt 1 ]; do
        echo "waiting for MC server"
	sleep 10
    done

fi

# Accessed by deactivate_minecraft.sh and PluginTA2SRIMinecraft.activateChannel ().

touch "$_PROXY_SENTINEL_FILE"
