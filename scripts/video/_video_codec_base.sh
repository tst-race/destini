# This file must be source'd and _not directly invoked_.

# Based upon jel2_Pixelfed_codec.sh

# Optional configuration symbols
#
#   TMP          location of log and temporary files; default: /tmp
#   KEEPTMP      if non-empty, do not remove "$TMP" files
#   LOGOUT       log re-direction command; default: "> /dev/null 2>&1"
#   WEDGE        base name of wedge app (e.g., pix_wedge, wedge.android, mjpeg_wedge, etc.)
#   UNWEDGE      base name of unwedge app (e.g., mjpeg_unwedge, pix_unwedge, unwedge.android, etc.); default: pix_unwedge
#   NOSIZECHECK  if non-empty, skip decode length verification
#
# Optional logging functions.  See code below.
#
#   _outlog      log re-direction function; takes as arguments a command line invocation (see default implementation below).
#
# Optional diagnostic functions (primarily used by jel2_Pixelfed_codec_android.sh).  See code below.
#
#   _hello0
#   _hello11
#   _altPaths
#   _hello2
#   _helloEncode
#   _encodeStatus
#

#############
# Functions #
#############

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

if ! type -t "_outlog" > /dev/null; then
_outlog()
{
    $@ > /dev/null 2>&1
}
fi

_usage ()
{
    _logError "$(basename $0) [options...] (encode | decode) ..."
    exit $1
}

_checkApps ()
{
    if [ ! -f "$_wedge" -o ! -f "$_unwedge" ]; then
        _logError 'ERROR: missing $_WEDGE or $_UNWEDGE applications.'
        exit 1
    fi
}

_decode ()
{
    # Recall that several unwedge parameters will be passed in via $@:
    "${_unwedge:-echo}" $@

    if [ $? -eq 243 ]; then
	return 0
    fi
}


_checkdecsize()
{
    _fsize=$(stat -c%s "$_vidDeOut")
    if [ $_fsize -gt 48 ]; then
        egrep -q '^bJeL' "$_vidDeOut"
        _status=$?
    else
        _status=48 # too short
    fi
}

_runFunc()
{
    local _func="$1"

    if type -t "$_func" > /dev/null; then
        $@
    fi
}


########
# Main #
########

_runFunc _hello0

_TMP=${TMP:-/tmp}

if [ -z "$TMP" -a -d "/log" ]; then
    _wedgeLog="/log/vidwedge.log"
    _unwedgeLog="/log/vidunwedge.log"
else
    _wedgeLog="$_TMP/vidwedge.log"
    _unwedgeLog="$_TMP/vidunwedge.log"
fi

_dirname=$(dirname $0)
export PATH="$_dirname/../bin:$PATH"

_runFunc _hello11

_WEDGE=${WEDGE:-pix_wedge}
_UNWEDGE=${UNWEDGE:-pix_unwedge}

_wedge=$(which $_WEDGE)
_unwedge=$(which $_UNWEDGE)

if [ -z "$_wedge" ]; then
    if type -t _altPaths > /dev/null; then
        _altPaths
    else
        _wedge=$_dirname/$_WEDGE
        _unwedge=$_dirname/$_UNWEDGE
    fi
fi

_runFunc _hello2

_checkApps

# https://unix.stackexchange.com/questions/14270/get-exit-status-of-process-thats-piped-to-another

_cmd=$1; shift

case "$_cmd" in

    encode)
        _vidEnIn="$_TMP/vidEnIn$$.msg"
        _vidEnOut="$_TMP/vidEnOut$$.mp4"

        _logEnc "$0 $@"
        cat > "$_vidEnIn"

        _runFunc _helloEncode $@
        _logEnc "msgIn: $(wc -c "$_vidEnIn") ($_vidEnIn)"

        # Encode/embed the non-empty message

        if [ -s "$_vidEnIn" ]; then

	    # EMBED THE MESSAGE IN THE COVER:
            _outlog "$_wedge" -message "$_vidEnIn" -output "$_vidEnOut"  $@
	    # echo  "$_wedge" -message "$_vidEnIn" -output "$_vidEnOut" $@ 
	    # "$_wedge" -message "$_vidEnIn" -output "$_vidEnOut" $@ >& /tmp/encode.log
            _status=$?

            _runFunc _encodeStatus

            if [ $_status -eq 0 ]; then
                cat $_vidEnOut
                _logEnc "msgOut: $(wc -c "$_vidEnOut") ($_vidEnOut)"
            fi

    	# Accommodate empty message

    	else
    	    _vidCover="${@:$#}"
    	    cat $_vidCover
    	    _status=$?
            _logEnc "coverOut: ($_vidCover)"
    	fi

        if [ -z "$KEEPTMP" ]; then
            rm -f $_TMP/$$_* $_vidEnIn $_vidEnOut
        fi

        cat << EOF >> "$_wedgeLog"
$(date +%c) exit $_status

EOF
        ;;

    decode)
        _vidDeIn="$_TMP/vidDeIn$$.jpg"
        _vidDeOut="$_TMP/vidDeOut$$.msg"

        _logDec "$0 $@"

        cat > "$_vidDeIn"

        # EXTRACT THE MESSAGE FROM THE STEGGED MEDIA:
        _outlog _decode $@ -steg "$_vidDeIn" -message "$_vidDeOut"
        _status=$?

        if [ $_status -eq 0 -a -z "$NOSIZECHECK" ]; then
            _checkdecsize
        fi

        if [ $_status -eq 0 ]; then
            cat "$_vidDeOut"
        fi

        cat << EOF >> "$_unwedgeLog"
$(date +%c) exit $_status

EOF
        if [ -n "$KEEPTMP" ]; then
            _logDec "  $_vidDeIn $_vidDeOut"
        else
            rm -f "$_vidDeIn" "$_vidDeOut"
        fi
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
