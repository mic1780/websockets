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

usage()
{
cat << EOF
usage: $0 options

This script is used when you execute a run executable

OPTIONS:
   -h                               Displays this help
   -b [ <name> ]                    Compile main application
   -f [ file1 [ file2 ... ] ]       Compile functions
   -l [ file1 [ file2 ... ] ]       Compile dynamic libraries
   -r [ <name> ]                    Runs the specified program after setup
EOF
}

while true; do
	case $1 in
		-h|--help)
			usage
			exit 1
			;;
		-b)
			COMPILE_APP=1
			shift
			if [[ $1 =~ ^-. ]]; then
				COMPILE_APP_NAME=""
			elif [ "$1" == "" ]; then
				COMPILE_APP_NAME=""
			else
				COMPILE_APP_NAME="$1"
				shift
			fi
			;;
		-f)
			COMPILE_FUNCTION=1
			while true; do
				shift
				if [[ $1 =~ ^-. ]]; then
					break
				elif [ "$1" == "" ]; then
					break
				else
					COMPILE_FUNCTION_NAMES="$COMPILE_FUNCTION_NAMES$1 "
				fi
			done
			
			if [ "$COMPILE_FUNCTION_NAMES" == "" ]; then
				COMPILE_FUNCTION_NAMES="libfunctions doFunction"
			fi
			;;
		-l)
			COMPILE_LIB=1
			while true; do
				shift
				if [[ $1 =~ ^-. ]]; then
					break
				elif [ "$1" == "" ]; then
					break
				else
					COMPILE_LIB_NAMES="$COMPILE_LIB_NAMES$1 "
				fi
			done
			
			if [ "$COMPILE_LIB_NAMES" == "" ]; then
				COMPILE_LIB_NAMES="sendMessage alterStruct performAction"
			fi
			;;
		-r)
			RUN_APP=1
			shift
			if [[ $1 =~ ^-. ]]; then
				RUN_APP_NAME=""
			elif [ "$1" == "" ]; then
				RUN_APP_NAME=""
			else
				RUN_APP_NAME="run $1"
				shift
			fi
			;;
		-*)
			echo "$0: Invalid option $1" >&2
			exit 2
			;;
		*)
			break
			;;
	esac
done

#compile functions
if [ "$COMPILE_FUNCTION" == "1" ]; then
	./compileFunctions.sh "$COMPILE_FUNCTION_NAMES"
fi

#compile libraries
if [ "$COMPILE_LIB" == "1" ]; then
	./compileLibs.sh "$COMPILE_LIB_NAMES"
fi

#compile main application
if [ "$COMPILE_APP" == "1" ]; then
	./compileApp.sh "$COMPILE_APP_NAME"
fi

#run app
if [ "$RUN_APP" == "1" ]; then
	./setupEnv.sh "$RUN_APP_NAME"
fi

#its all over now!
echo ""
echo "Script is now finished"
