#!/bin/bash

if [[ ( -z $1 ) || (-z $(realpath $1) ) ]]; then
	echo "incorrect <path> parameter"
	exit 1
fi
echo -n "in path [$1] create "

if [[ -z $2 ]]; then
	echo "incorrect <file_name> parameter"
	exit 1
fi

echo "" > "$1/$2.h"
