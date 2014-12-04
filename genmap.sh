#!/bin/bash

MAP_WIDTH=50
MAP_HEIGHT=50

if [[ ! -z $1 ]]; then #arg1 exists
    if [[ $1 =~ ^-?[0-9]+$ ]]; then #arg is an integer
	MAP_WIDTH=$1
    fi
fi

if [[ ! -z $2 ]]; then #arg2 exists
    if [[ $2 =~ ^-?[0-9]+$ ]]; then # regex to check that arg is an integer
	 MAP_HEIGHT=$2
    fi
fi

function genPseudoRandomASCII {
    # get chars from urandom, suppress error output and pipe to base64 to get rid of escape chars
    local RAND_CHAR=$(base64 /dev/urandom | head -c 1)
    echo -n $RAND_CHAR
}

function getValOrSpace {
	local CHAR=$(genPseudoRandomASCII)
	local SPACE=" "

	local COIN=$RANDOM
	local PUT_SPACE=$(( $COIN % 2))

	if [[ $PUT_SPACE -ne 0 ]]; then
	    CHAR="$SPACE"
	fi

	echo -n "$CHAR"
}

function genMap {
	MAP_STR=""

	for((i=0; i<$MAP_HEIGHT; i++)) do
		for((j=0; j<$MAP_WIDTH; j++)) do
			VAR="$(getValOrSpace)"
			printf -v line "%c" "$VAR"
			MAP_STR="$MAP_STR$line"
		done
		printf -v line "\n"
		MAP_STR="$MAP_STR$line"
	done

	echo -n "$MAP_STR"
}

echo "One moment, generating map..."
MAP_STR=$(genMap)
REAL_CHARS=$(echo -n $MAP_STR | tr -cd ' ' | wc -c)
MIN_CHARS=$(( ($MAP_HEIGHT * $MAP_WIDTH) / 4))
ITR=1

while [ "$REAL_CHARS" -lt "$MIN_CHARS" ]
do
    if [[ $ITR == 10 ]]; then
		>&2 echo "Couldn't Generate a file!"
		exit -1
    fi

	echo "One moment, generating map..."
	MAP_STR=$(genMap)

    REAL_CHARS=$(echo -n $MAP_STR | tr -cd ' ' | wc -c)
    ITR=$(( $ITR + 1))
done

>&1 echo "$MAP_STR"
