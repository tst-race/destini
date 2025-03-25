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

ANDROID_ARM=1
ANDROID_x86=1
LINUX_ARM=1
LINUX_x86=1

. ${BASE_DIR}/build_specific_artifacts.sh $@
