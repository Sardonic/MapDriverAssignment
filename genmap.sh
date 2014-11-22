#!/bin/bash

MAP_WIDTH=50
MAP_HEIGHT=50

function genPseudoRandomASCII {
    # get chars from urandom, suppress error output and pipe to base64 to get rid of escape chars
    local RAND_CHAR=$(base64 /dev/urandom | head -c 1)
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

echo "One moment, generating map..."
MAP_STR=""

for((i=0; i<$MAP_HEIGHT; i++)) do
    for((j=0; j<$MAP_WIDTH; j++)) do
	VAR=$(genPseudoRandomASCII)
	printf -v line "$VAR" $VAR
	MAP_STR="$MAP_STR$line"
    done
    printf -v line '\n'
    MAP_STR="$MAP_STR$line"
done

NUM_SPACES=$(echo $MAP_STR | tr -cd ' ' | wc -c) # for some reason tr is counting the newlines
MAX_SPACES=$(( $MAP_HEIGHT + $MAP_HEIGHT * $MAP_WIDTH / 2)) # compensates for the tr thing, check for more spaces
ITR=1

while [ $NUM_SPACES -gt $MAX_SPACES ]
do
    if [[ $ITR == 10 ]]; then
	>&2 echo "Couldn't Generate a file!"
	exit -1
    fi

    for((i=0; i<$MAP_HEIGHT; i++)) do
	for((j=0; j<$MAP_WIDTH; j++)) do
	    VAR=$(genPseudoRandomASCII)
	    printf -v line "%c" $VAR
	    MAP_STR="$MAP_STR$line"
	done
	printf -v line '\n'
	MAP_STR="$MAP_STR$line"
    done

    NUM_SPACES=$(echo $MAP_STR | tr -cd ' ' | wc -c) # for some reason tr is counting the newlines
    ITR=$(( $ITR + 1))
done

>&1 echo "$MAP_STR"
