#!/bin/bash

function genPseudoRandomASCII {
    # get chars from urandom, suppress error output and pipe to base64 to get rid of escape chars
    local RAND_CHAR=$(dd if=/dev/urandom bs=1 count=1 2> /dev/null | base64)

    COIN=$RANDOM
    let "COIN %= 2"
    if [[ $COIN -eq 0 ]]; then
	RAND_CHAR='_'
    fi
    
    echo $RAND_CHAR
}

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

NUM_LOOPS=$($MAP_HEIGHT * $MAP_WIDTH + $MAP_HEIGHT)
for((i=0; i<; i++)) do
    VAR=$(genPseudoRandomASCII)
    MAP[i]=$VAR
    printf "%c " $VAR

    i++
    i_MOD_WIDTH=$($i % $MAP_WIDTH)
    if [[ $i_MOD_WIDTH -eq 0 ]]; then
	i++
	MAP[i]='\n'
    fi
done
