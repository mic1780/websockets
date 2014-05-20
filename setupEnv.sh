#!/bin/bash

if [ "$SYSTEMDRIVE\\Windows" == "$SYSTEMROOT" ]; then
	if [ "$LIBRARRY_PATH" == "" ]; then
		unset LIBRARY_PATH
		export LIBRARY_PATH=$(pwd)/lib:$LIBRARY_PATH
	fi
#	if [ "$PATH" == "$ORIGINAL_PATH" ]; then
		export PATH=$ORIGINAL_PATH:$(pwd)/lib
#	fi
else
	if [ "$LD_LIBRARRY_PATH" == "" ]; then
		unset LD_LIBRARY_PATH
		export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
	fi
fi

if [ "$1" == "run" ]; then
	if [ "$2" != "" ]; then
		./$2
	fi
fi
