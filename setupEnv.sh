#!/bin/bash
# 
#   Copyright (C) 2014  Michael Cummins
#   License: GNUv2
#   
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 

if [ "$OS" == "Windows_NT" ]; then
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
		export LD_LIBRARY_PATH=$(pwd)/so:$LD_LIBRARY_PATH
	fi
fi

if [ "$1" == "run" -a "$2" != "" ]; then
	./$2
fi
