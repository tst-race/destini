#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Script to build specific artifacts for the plugin in these environments: 
# android client, linux client, and linux server. Once built, move the artifacts
# to the plugin/artifacts dir for publishing to Jfrog Artifactory
# -----------------------------------------------------------------------------


set -e
CALL_NAME="$0"


###
# Helper functions
###

_CMAKE_BUILD()		# <target>
{
    local _target="$1"; shift
    if [ -n "${!_target}" ]; then
	local _platform=$(echo "$_target" | cut -d_ -f1)
	local _cPlatform=$(echo "$_platform" | tr '[A-Z]' '[a-z]')
	local _dPlatform
	local _role
	local _type
	case "$_platform" in
	    ANDROID)
		_dPlatform=Android
		_role=Client
		_type=client
		;;
	    LINUX)
		_dPlatform=Linux
		_role='Client/Server'
		_type='[client|server]'
		;;
	esac

	local _dProcessor=$(echo "$_target" | cut -d_ -f2)
	local _processor=$(echo "$_dProcessor" | sed -e 's/x86/x86_64/' -e 's/ARM/arm64-v8a/')

	local _plugin="${_cPlatform}-${_processor}-${_type}"
	local _preset="${_platform}_${_processor}"

#cat <<EOF
	formatlog "INFO" "Building $_dPlatform $_dProcessor $_role"
	cmake $@ --preset=${_preset} -Wno-dev \
	      -DBUILD_VERSION="${RACE_VERSION}-${PLUGIN_REVISION}" \
	      $VERBOSE
	# This will copy the output to plugin/artifacts/$_plugin
	cmake $@ --build --preset=${_preset} $VERBOSE
#EOF
    fi
}


# Load Helper Functions
BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
. ${BASE_DIR}/helper_functions.sh


###
# Arguments
###

# Version values
RACE_VERSION="2.4.2"
PLUGIN_REVISION="r1"

# Build Arguments
CMAKE_ARGS=""
VERBOSE=""

HELP=\
"Script to build artifacts for the plugin for all possible environments: 
android client, linux client, and linux server. Once built, move the artifacts
to the plugin/artifacts dir for publishing to Jfrog Artifactory
NOTE: run from race-sdk container 

Build Arguments:
    -c [value], --cmake_args [value], --cmake_args=[value]
        Additional arguments to pass to cmake.
    --race-version [value], --race-version=[value]
        Specify the RACE version. Defaults to '${RACE_VERSION}'.
    --plugin-revision [value], --plugin-revision=[value]
        Specify the Plugin Revision Number. Defaults to '${PLUGIN_REVISION}'.
    --verbose
        Make everything very verbose.

Help Arguments:
    -h, --help
        Print this message

Examples:
    ./build_artifacts.sh --race-version=1.4.0
"

while [ $# -gt 0 ]
do
    key="$1"

    case $key in
        --race-version)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing RACE version number" >&2
            exit 1
        fi
        RACE_VERSION="$2"
        shift
        shift
        ;;
        --race-version=*)
        RACE_VERSION="${1#*=}"
        shift
        ;;
        
        --plugin-revision)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing revision number" >&2
            exit 1
        fi
        PLUGIN_REVISION="$2"
        shift
        shift
        ;;
        --plugin-revision=*)
        PLUGIN_REVISION="${1#*=}"
        shift
        ;;

        --verbose)
        VERBOSE="-DCMAKE_VERBOSE_MAKEFILE=ON"
        shift
        ;;

	--)
	    shift
	    break
	    ;;

        -h|--help)
        printf "%s" "${HELP}"
        shift
        exit 1;
        ;;
        *)
        formatlog "ERROR" "${CALL_NAME} unknown argument \"$1\""
        exit 1
        ;;
    esac
done

if [ ! -z "${VERBOSE}" ] ; then
    set -x
fi

if [ -z "${APP_ROOT}" ] ; then
    formatlog "ERROR" "Script should be run in a race-sdk image (or called through build_artifacts_in_docker_image.sh). Exiting"
    exit 1
fi

###
# Main Execution
###

# Verify race-cmake-modules repository is properly placed and configured
if [ -z "$(ls ${BASE_DIR}/cmake/*)" ]; then
    formatlog "ERROR" "race-cmake-modules not found. Please run: prepare_cmake_modules.sh"
    exit 1
fi

formatlog "INFO" "Cleaning plugin/artifacts before building artifacts"
bash ${BASE_DIR}/clean_artifacts.sh

if [ "$(uname -m)" == "x86_64" ]
then
    _CMAKE_BUILD ANDROID_x86
    _CMAKE_BUILD ANDROID_ARM
else 
    echo "Android builds not yet supported on arm64 hosts"
fi

_CMAKE_BUILD LINUX_x86
_CMAKE_BUILD LINUX_ARM
