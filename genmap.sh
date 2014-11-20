#!/bin/bash

MAP_WIDTH=50
MAP_HEIGHT=50

function genPseudoRandomASCII {
    # get chars from urandom, suppress error output and pipe to base64 to get rid of escape chars
    local RAND_CHAR=$(dd if=/dev/urandom bs=1 count=1 2> /dev/null | base64)
    echo $RAND_CHAR
}

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

for((i=0; i<$MAP_HEIGHT; i++)) do
    for((j=0; j<$MAP_WIDTH; j++)) do
	VAR=$(genPseudoRandomASCII)
	printf "%c" $VAR
    done
    printf '\n'
done
