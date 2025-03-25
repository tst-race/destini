#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Pull and prepare race-cmake-modules for use with the CMake build
#
# Arguments:
# --branch[value], --branch=[value]
#     Branch of race-cmake-modules to pull
# --git-user [value], --git-user=[value]
#     git username to pull race-cmake-modules if necessary
# --git-token [value], --git-token=[value]
#     git token to pull race-cmake-modules if necessary
# --repo-path [value], --repo-path=[value]
#     path to race-cmake-modules. will create a symlink and use that dir instead of cloning
# -h, --help
#     Print help and exit
#
# Example Call:
#    ./prepare_cmake_modules.sh \
#        --branch=v2 \
#        {--help}
# -----------------------------------------------------------------------------


set -e


###
# Helper functions
###


# Load Helper Functions
CURRENT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
. ${CURRENT_DIR}/helper_functions.sh


###
# Arguments
###

# Dest Path
DEST_PATH="${CURRENT_DIR}/cmake"

# Branch
GIT_BRANCH="v2"

# Git User Overrides (for CI)
GIT_USER=
GIT_TOKEN=

# Local race-cmake-modules path
REPO_PATH=

# Parse CLI Arguments
while [ $# -gt 0 ]
do
    key="$1"

    case $key in
        --branch)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing --branch" >&2
            exit 1
        fi
        GIT_BRANCH="$2"
        shift
        shift
        ;;
        --branch=*)
        GIT_BRANCH="${1#*=}"
        shift
        ;;

        --git-user)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing --git-user" >&2
            exit 1
        fi
        GIT_USER="$2"
        shift
        shift
        ;;
        --git-user=*)
        GIT_USER="${1#*=}"
        shift
        ;;

        --git-token)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing --git-token" >&2
            exit 1
        fi
        GIT_TOKEN="$2"
        shift
        shift
        ;;
        --git-token=*)
        GIT_TOKEN="${1#*=}"
        shift
        ;;

        --repo-path)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing --repo-path" >&2
            exit 1
        fi
        REPO_PATH="$2"
        shift
        shift
        ;;
        --repo-path=*)
        REPO_PATH="${1#*=}"
        shift
        ;;

        -h|--help)
        echo "Example Call: bash prepare_cmake_modules.sh --branch=$GIT_BRANCH {--help}"
        exit 1;
        ;;

        *)
        formatlog "ERROR" "unknown argument \"$1\""
        exit 1
        ;;
    esac
done


###
# Main Execution
###


# Cloning the race-cmake-modules or creating symlink
if [ ! -d "${DEST_PATH}" ]; then
    if [ ! -z "${REPO_PATH}" ]; then
        formatlog "INFO" "REPO_PATH set (${REPO_PATH}), creating symlink"
        ln -s "${REPO_PATH}" "${DEST_PATH}"
    elif [ -z "${GIT_USER}" ]; then
        formatlog "INFO" "GIT_USER not set; cloning with ssh"
        git clone --single-branch --depth=1 --branch="${GIT_BRANCH}" "git@gitlab.race.twosixlabs.com:race-common/race-cmake-modules.git" "${DEST_PATH}"
    else
        formatlog "INFO" "GIT_USER set (${GIT_USER}), cloning with user and token"
        git clone --single-branch --depth=1 --branch="${GIT_BRANCH}" "https://${GIT_USER}:${GIT_TOKEN}@gitlab.race.twosixlabs.com/race-common/race-cmake-modules.git" "${DEST_PATH}"
    fi
else
    formatlog "INFO" "race-cmake-modules is already prepared"
fi
