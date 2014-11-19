#!/bin/bash

MAP_WIDTH=50
MAP_HEIGHT=50

function genPseudoRandomASCII {
    local RAND_CHAR=$[RANDOM % 94 + 32] # generate int between 32 and 126 (good ascii chars)
    RAND_CHAR=$(printf \\$(printf '%03o' $((RAND_CHAR)))) # make it a char
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
	printf "%c " $VAR
    done
    printf '\n'
done
