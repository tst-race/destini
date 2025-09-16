#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Script to build artifacts for the plugin in all possible environments: 
# android client, linux client, and linux server. Once built, move the artifacts
# to the plugin/artifacts dir for publishing to Jfrog Artifactory
# -----------------------------------------------------------------------------


set -e
CALL_NAME="$0"


###
# Helper functions
###


# Load Helper Functions
BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
. ${BASE_DIR}/helper_functions.sh


###
# Arguments
###

# Version values
RACE_VERSION="2.3.0"
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
    ./build.sh --race-version=1.4.0
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

export CC=clang
export CXX=clang++

#if [ -z "${APP_ROOT}" ] ; then
#    formatlog "ERROR" "Script should be run in a race-sdk image (or called through build_artifacts_in_docker_image.sh). Exiting"
#    exit 1
#fi

###
# Main Execution
###

# Verify race-cmake-modules repository is properly placed and configured
if [ -z "$(ls ${BASE_DIR}/cmake/*)" ]; then
    formatlog "ERROR" "race-cmake-modules is not found. Please run: prepare_cmake_modules.sh"
    exit 1
fi

apt-get -y update
apt-get -y install libboost-all-dev
apt-get -y install libcpprest-dev
# apt-get -y install libxml2-dev
apt-get -y install libjsoncpp-dev
apt-get -y install libjpeg62-dev
apt-get -y install libevent-dev
# # RISKY!  Do we know that this version has everything we need??
apt-get -y install ffmpeg
apt-get -y install parallel

pip install pillow
pip install psutil

# pushd /usr/local/include
# ls /usr/include
# ln -s /usr/include/libxml2/libxml
# popd

formatlog "INFO" "Cleaning plugin/artifacts Before Building Artifacts"
bash ${BASE_DIR}/clean_artifacts.sh

LINUX_PRESET=LINUX_x86_64
ARCH=x86_64
if [ "$(uname -m)" == "aarch64" ] || [ "$(uname -m)" == "arm64" ]
then
    LINUX_PRESET=LINUX_arm64-v8a
    ARCH=arm64-v8a
fi

# comment arm or x86 linux builds as needed for host architecture
formatlog "INFO" "Building ${LINUX_PRESET} Linux Client/Server"
cmake --preset=$LINUX_PRESET \
    -DBUILD_VERSION="local" --debug-output -DCXX="clang++ -std=c++17"
# This will copy the output to plugin/artifacts/linux-x86_64-[client|server]
cmake --build --preset=$LINUX_PRESET

for NODETYPE in server client; do
    cp jpeg-covers.tar kit/artifacts/linux-${ARCH}-${NODETYPE}/PluginDestini/
    cp -r scripts/ kit/artifacts/linux-${ARCH}-${NODETYPE}/PluginDestini/
    cp -r bin/ kit/artifacts/linux-${ARCH}-${NODETYPE}/PluginDestini/
done


# TODO: Android Support - build error currently
# # *** TEMPORARY HACK: '==' -> '!=' to accommodate lack of jsoncpp on Android ***
# if [ "$(uname -m)" == "x86_64" ]
# then
#     formatlog "INFO" "Building Android x86_64 Client"
#     cmake --preset=ANDROID_x86_64 -Wno-dev \
#         -DBUILD_VERSION="${RACE_VERSION}-${PLUGIN_REVISION}"
#     # This will copy the output to plugin/artifacts/android-x86_64-client
#     cmake --build --preset=ANDROID_x86_64

#     formatlog "INFO" "Building Android arm64-v8a Client"
#     cmake --preset=ANDROID_arm64-v8a -Wno-dev \
#         -DBUILD_VERSION="${RACE_VERSION}-${PLUGIN_REVISION}"
#     # This will copy the output to plugin/artifacts/android-arm64-v8a-client
#     cmake --build --preset=ANDROID_arm64-v8a
# else 
#     echo "android builds not yet supported on arm64 hosts"
# fi

#
# Should expand this and use conditionals as in race-core/build.sh:
#

#formatlog "INFO" "Building Linux Client/Server"
#cmake --preset=LINUX_arm64-v8a -Wno-dev \
#    -DBUILD_VERSION="${RACE_VERSION}-${PLUGIN_REVISION}"
# This will copy the output to plugin/artifacts/linux-arm64-v8a-[client|server]
#cmake --build --preset=LINUX_arm64-v8a
