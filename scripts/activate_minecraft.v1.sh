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
# Arguments
###


# Parse CLI Arguments
while [ $# -gt 0 ]
do
    key="$1"

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
# Main Execution
###

cd ${BASE_DIR}

# Print the dummy log as a test
cat ${BASE_DIR}/dummy.conf

if [ ! -f /usr/local/lib/_IOManager.so ]; then
    echo "hello from start internal services" >> /tmp/iservice.logxs
    # pip install Flask
fi


if [ ! -f /tmp/mc_init ]; then
    touch /tmp/mc_init
    
    
    #apt-get install -y xvfb x11-apps software-properties-common python3-tk python3-dev
    #pip3 install pyautogui
	    
    #    cp -r /aux_data/PluginTA2SRIMinecraft/Minecraft-cached /usr/local/lib/race/ta2/PluginTA2SRIMinecraft/Minecraft-cached
    cp -r /aux_data/PluginTA2SRIMinecraft/Minecraft-cached /Minecraft-cached
    export PATH=${JAVA_HOME}/bin:${PATH}  
    # export MC_PROXY_DIR=/usr/local/lib/race/ta2/PluginTA2SRIMinecraft/MinecraftProxy
    export MC_SVR_IP="race-server-00001" #(must be resolvable name)
    export MD_HOST=$MC_SVR_IP #(must be resolvable name)
    export MC_PROXY_PORT=11011 #(must be resolvable name)
    export NO_MC_MEMCK=1


    # IP or hostname?
    # doesn't have to match config
    export PERSONA=`hostname` # hostname | nslookup


    if [ $PERSONA = $MC_SVR_IP ]; then

	cd /Minecraft-cached/Java_NIO_SOCKS/src
	javac SOCKS/SOCKSProxy.java
	java SOCKS.SOCKSProxy > /log/mc-server-socks.log 2>&1 &

        cd /Minecraft-cached/mc2_server
	java -Xmx1024M -Xms1024M -jar forge-1.15.2-31.2.52.jar nogui > /log/mcserver.log 2>&1 &

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

	cd /Minecraft-cached/Java_NIO_SOCKS_Client/src
	javac SOCKS/SOCKSProxy.java
	java SOCKS.SOCKSProxy 11011 22022 > /log/mc-client-socks.log 2>&1 &
	
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


    fi
fi

