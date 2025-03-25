#!/usr/bin/env bash


set -e
CALL_NAME="$0"


###
# Helper functions
###


# Load Helper Functions
BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)

ANDROID_x86=1

. ${BASE_DIR}/build_specific_artifacts.sh
