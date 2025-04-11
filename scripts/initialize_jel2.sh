#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Initialize JEL2 encoder
# -----------------------------------------------------------------------------


###
# Helper variables
###

_logMsg()
{
    echo "$(date) $0: $1"
    # >> /log/destini.log
}

_logMsg "Entering..."

BASE_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) >/dev/null 2>&1 && pwd)
TOP_DIR="${BASE_DIR}/.."
COVERS_TGZ="covers/sri-ta2.tar"

_logMsg "$BASE_DIR $TOP_DIR $COVERS_TGZ"

###
# Main Execution
###

_logMsg "pwd before: $(pwd)"

cd ${TOP_DIR}

_logMsg "pwd after cd: $(pwd)"

if [ -f .jel2_init -o ! -f "${COVERS_TGZ}" ]; then
    _logMsg "early termination."
    while [ ! -f .jel2_init_complete ]; do
	sleep 1
    done

    exit 0
fi

touch .jel2_init

for _sDir in bin scripts; do
    find $_sDir -type f -exec chmod a+x {} \;
done

_logMsg "Before tar -xf"

tar -xf ${COVERS_TGZ}
sed -i -Ee 's|^/ramfs/sriPixelfed/||' sri-ta2/covers/jpeg/capacities.txt

find sri-ta2 -type d -exec chmod o+rx {} \;
find sri-ta2 -type f -exec chmod o+r {} \;

touch .jel2_init_complete

_logMsg "terminating."
