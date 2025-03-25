#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Push a set of artifacts to jfog.
#
# Note, it is expected that you have Jfrog already configured to interact
# with https://jfrog.race.twosixlabs.com/. please run `jr
#
# Arguments:
# --race-version [value], --race=[value]
#     Specify the RACE version of the build to push
# --plugin-revision [value], --plugin-revision=[value]
#     Specify the revision of the build to push
# --plugin-id [value], --plugin-id=[value]
#     Specify the pluginId
# --env [value], --env=[value]
#     Env (dev|prod) to push artifacts to in artifactory
# -h, --help
#     Print help and exit
#
# Example Call:
#    ./push_plugin_to_artifactory.sh \
#        --race-version=1.4.0 \
#        --plugin-revision=r1 \
#        {--help}
# -----------------------------------------------------------------------------


set -ex


###
# Helper functions
###


# Load Helper Functions
CURRENT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
. ${CURRENT_DIR}/helper_functions.sh


###
# Arguments
###

# Artifactory Values
PERFORMER="twosix"
ENV="dev"
BUILD_NUMBER=

# Plugin Value
PLUGIN_ID="PluginTa2TwoSixStubDecomposed"

# Version values
RACE_VERSION="2.3.0"
PLUGIN_REVISION="r1"

# Parse CLI Arguments
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
        
        --build-number)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing build number" >&2
            exit 1
        fi
        BUILD_NUMBER="$2"
        shift
        shift
        ;;
        --build-number=*)
        BUILD_NUMBER="${1#*=}"
        shift
        ;;

        --plugin-id)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing pluginId" >&2
            exit 1
        fi
        PLUGIN_ID="$2"
        shift
        shift
        ;;
        --plugin-id=*)
        PLUGIN_ID="${1#*=}"
        shift
        ;;

        --env)
        if [ $# -lt 2 ]; then
            formatlog "ERROR" "missing env" >&2
            exit 1
        fi
        ENV="$2"
        shift
        shift
        ;;
        --env=*)
        ENV="${1#*=}"
        shift
        ;;

        -h|--help)
        echo "Example Call: bash push_plugin_to_artifactory.sh --race=1.4.0 --revision=r1 {--help}"
        exit 1;
        ;;

        *)
        formatlog "ERROR" "unknown argument \"$1\""
        exit 1
        ;;
    esac
done

ARTIFACTORY_REPO="${PERFORMER}-generic-${ENV}-local"
ARTIFACTORY_PLUGIN_PATH="${ARTIFACTORY_REPO}/race/${RACE_VERSION}/ta2/${PLUGIN_ID}/${PLUGIN_REVISION}"

BUILD_NAME="${ARTIFACTORY_PLUGIN_PATH}"
if [ -z "${BUILD_NUMBER}" ]; then
    BUILD_NUMBER="0"
fi

###
# Main Execution
### 


# Verify Jfrog Configuration (Fails if Not Configured)
jfrog config use race

# Navigate to Plugin Dir
cd ${CURRENT_DIR}/plugin

# Verify race-python-utils reposiroty is properly placed and configured
if [ -z "$(ls ${CURRENT_DIR}/plugin/channels/race-python-utils/*)" ]; then
    formatlog "ERROR" "race-python-utils is not found. Please run: prepare_python_utils.sh or manually prepare race-python-utils"
    exit 1
fi

# Verify Artifacts Exist
ls artifacts/android-x86_64-client/"${PLUGIN_ID}"/*.so
ls artifacts/android-arm64-v8a-client/"${PLUGIN_ID}"/*.so
ls artifacts/linux-x86_64-client/"${PLUGIN_ID}"/*.so
ls artifacts/linux-x86_64-server/"${PLUGIN_ID}"/*.so
ls artifacts/linux-arm64-v8a-client/"${PLUGIN_ID}"/*.so
ls artifacts/linux-arm64-v8a-server/"${PLUGIN_ID}"/*.so

# Upload the plugin dir to the specified location in Artifactory
jfrog rt upload \
    "artifacts/*" \
    "${ARTIFACTORY_PLUGIN_PATH}/" \
    --build-name="${BUILD_NAME}" \
    --build-number="${BUILD_NUMBER}" \
    --flat=false \
    --recursive=true \
    --exclusions="*DS_Store;*gitignore"

# Upload the twoSixIndirectComposition
jfrog rt upload \
    "channels/twoSixIndirectComposition/*" \
    "${ARTIFACTORY_PLUGIN_PATH}/" \
    --build-name="${BUILD_NAME}" \
    --build-number="${BUILD_NUMBER}" \
    --flat=false \
    --recursive=true \
    --exclusions="*DS_Store;*gitignore;channels/twoSixIndirectComposition/configs/*"

# Upload race-python-utils (used to generate configs)
jfrog rt upload \
    "channels/race-python-utils/*" \
    "${ARTIFACTORY_PLUGIN_PATH}/" \
    --build-name="${BUILD_NAME}" \
    --build-number="${BUILD_NUMBER}" \
    --flat=false \
    --exclusions="*DS_Store;*gitignore;*__pycache__*;*.git*;*scripts*;*.blackrc.toml;*.coveragerc;*.pycodestylerc;*.pylintrc;*Makefile;*README.md;*setup.cfg;*setup.py;*race_python_utils/tests/*;*race_python_utils/ta1/tests/*"

if [[ "$BUILD_NUMBER" != "0" ]]; then
    # Collect environment variables and add them to the build info:
    jfrog rt build-collect-env $BUILD_NAME $BUILD_NUMBER

    # Publish the build info to Artifactory:
    jfrog rt build-publish $BUILD_NAME $BUILD_NUMBER
fi
