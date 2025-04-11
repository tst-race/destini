#!/bin/bash

if [ -d "/log" ]; then
    _wedgeLog="/log/wedge.log"
    _unwedgeLog="/log/unwedge.log"
else
    _wedgeLog="/tmp/wedge.log"
    _unwedgeLog="/tmp/unwedge.log"
fi


__log ()
{
    echo "$(date): $1" >> "$2"
}

_logEnc ()
{
    __log "$1" "$_wedgeLog"
}

_logDec ()
{
    __log "$1" "$_unwedgeLog"
}

_logError ()
{
    echo "$1" 1>&2
}

_usage ()
{
    _logError "$(basename $0) [options...] (encode | decode) ..."
    exit $1
}

_checkApps ()
{
    if [ ! -f "$_wedge" ] || [ ! -f "$_unwedge" ]; then
        _logError 'ERROR: missing wedge or unwedge applications.'
        exit 1
    fi
}

_dirname=$(dirname $0)
export PATH="$_dirname/../bin:$PATH"

_decode ()
{
    "${_unwedge:-echo}" $*
}


_wedge=$(which wedge.imgur)
_unwedge=$(which unwedge.imgur)

if [ -z "$_wedge" ]; then
    _wedge=$_dirname/wedge.imgur
    _unwedge=$_dirname/unwedge.imgur
fi

_checkApps

_cmd=$1; shift

# https://unix.stackexchange.com/questions/14270/get-exit-status-of-process-thats-piped-to-another

case "$_cmd" in

    encode)
	_logEnc "$0 $*"
	cat > "/tmp/$$_msgIn"
	_logEnc "msgIn: $(wc -c /tmp/$$_msgIn)"	
        "$_wedge" -data "/tmp/$$_msgIn" $*  "/tmp/$$_steg.jpg" > /dev/null 2>&1
	_status=$?
	
	if [ $? -eq 0 ]; then
	    cat /tmp/$$_steg.jpg
	else
	    _status=11
	fi

        cat << EOF >> "$_wedgeLog"
exit $_status

EOF
        ;;

    decode)
        _jDeIn="/tmp/jDeIn$$.jpg"
	_jDeOut="/tmp/jDeOut$$.msg"

        _logDec "$0 $*"

        cat > "$_jDeIn"

        _decode $* "$_jDeIn" "$_jDeOut" > /dev/null 2>&1
	_status=$?
	
	if [ $_status -eq 0 ]; then
	    cat "$_jDeOut"
	fi
	
        cat << EOF >> "$_unwedgeLog"
exit $_status

EOF
        rm -f "$_jDeIn" "$_jDeOut"
        ;;

    -h|--help)
        _usage 0
        ;;

    *)
        _logError "ERROR: missing or unrecognized command: \"$_cmd\""
        _usage 1
        ;;
esac

exit $_status
