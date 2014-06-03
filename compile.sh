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
   -h, --help                       Displays this help
   -b                               Create a new application (compile with default commands)
   -d                               Delete executables and libraries
   -a [ <name> ]                    Compile main application
   -f [ file1 [ file2 ... ] ]       Compile functions
   -l [ file1 [ file2 ... ] ]       Compile dynamic libraries
   -r [ <name> ]                    Runs the specified program after setup
EOF
}

function contains()
{
	local haystack=$1
	local neddle=$2
	for i in $haystack; do
		if [ "$i" == "$neddle" ]; then
			echo "1"
			return 1
		fi
	done
	
	echo "0"
	return 0
}

DEFAULT_FUNCTION_NAMES=("libfunctions")
DEFAULT_LIB_NAMES=("sendMessage" "alterStruct" "performAction" "callFunction")

while true; do
	case $1 in
		-h|--help)
			usage
			exit 1
			;;
		-a)
			COMPILE_APP=1
			COMPILE_APP_NAME=
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
		-b)
			NEW_BUILD=1
			shift
			DELETE_FILES=1
			COMPILE_FUNCTION=1
			COMPILE_LIB=1
			COMPILE_APP=1
			;;
		-d)
			DELETE_FILES=1
			shift
			;;
		-f)
			COMPILE_FUNCTION=1
			COMPILE_FUNCTION_NAMES=()
			while true; do
				shift
				if [[ $1 =~ ^-. ]]; then
					break
				elif [ "$1" == "" ]; then
					break
				else
					COMPILE_FUNCTION_NAMES[${#COMPILE_FUNCTION_NAMES[@]}]="$1"
				fi
			done
			;;
		-l)
			COMPILE_LIB=1
			COMPILE_LIB_NAMES=()
			while true; do
				shift
				if [[ $1 =~ ^-. ]]; then
					break
				elif [ "$1" == "" ]; then
					break
				else
					COMPILE_LIB_NAMES[${#COMPILE_LIB_NAMES[@]}]="$1"
				fi
			done
			;;
		-r)
			RUN_APP=1
			RUN_APP_NAME=""
			shift
			if [[ $1 =~ ^-. ]]; then
				RUN_APP_NAME=""
			elif [ "$1" == "" ]; then
				RUN_APP_NAME=""
			else
				RUN_APP_NAME="$1"
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

#new build
if [ "$NEW_BUILD" == "1" ]; then
	echo "Beginning full compile"
	echo ""
fi

#delete files
if [ "$DELETE_FILES" == "1" ]; then
	
	if [ "$NEW_BUILD" == "1" ]; then
		echo "Removing old files"
	else
		echo "Deleting compiled libraries and executables"
	fi
	
	if [ "$OS" == "Windows_NT" ]; then
		rm -rf lib/*.dll
		rm -rf bin/*.exe
		rm -f ./*.exe
	else
		rm -rf so/*.so
		rm -rf bin/*.out
		rm -f ./*.out
	fi
	
	rm -rf objects/*.o
	rm -rf ./tmp
fi


if [ "$COMPILE_FUNCTION" == "1" -a "${#COMPILE_FUNCTION_NAMES[@]}" == "0" ]; then
	COMPILE_FUNCTION_NAMES=${DEFAULT_FUNCTION_NAMES[@]}
fi

if [ "$COMPILE_LIB" == "1" -a "${#COMPILE_LIB_NAMES[@]}" == "0" ]; then
	COMPILE_LIB_NAMES=${DEFAULT_LIB_NAMES[@]}
fi


#compile functions
if [ "$COMPILE_FUNCTION" == "1" ]; then
	
	for i in ${COMPILE_FUNCTION_NAMES[@]}; do
		if [ $(contains "`echo ${DEFAULT_LIB_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -l $i)"
		else
			echo "Generating $i.o"
			gcc -c -o objects/$i.o $i.c
		fi
	done
	
	if [ "$OS" == "Windows_NT" ]; then
		echo "Generating libfunctions.dll"
		gcc -shared -o lib/libfunctions.dll objects/*.o
	else
		echo "Generating libfunctions.so"
		gcc -shared -o so/libfunctions.so objects/*.o
	fi
	
	echo "Finished"
	echo ""
	
fi

#compile libraries
if [ "$COMPILE_LIB" == "1" ]; then
	
	echo "Creating tmp directory"
	mkdir tmp
	
	for i in ${COMPILE_LIB_NAMES[@]}; do
		echo "Generating $i.o"
		gcc -c -o tmp/$i.o $i.c
		if [ $(contains "`echo ${DEFAULT_FUNCTION_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -f $i)"
		else
			if [ "$OS" == "Windows_NT" ]; then
				echo "Generating $i.dll"
				gcc -shared -o lib/lib$i.dll tmp/$i.o lib/libfunctions.dll
			else
				echo "Generating $i.so"
				gcc -shared -o so/lib$i.so tmp/$i.o so/libfunctions.so
			fi
		fi
	done
	
	echo "Cleaning up..."
	rm -r tmp
	echo "Finished"
	echo ""
fi

#compile main application
if [ "$COMPILE_APP" == "1" ]; then
	
#	if [ "$COMPILE_APP_NAME" == "run" -o "$COMPILE_APP_NAME" == "run.exe" -o "$COMPILE_APP_NAME" == "run.out" ]; then
#		echo "ERROR: Do not overwrite run executable."
#		exit
#	fi
	
	if [ "$OS" == "Windows_NT" ]; then
		if [ "$COMPILE_APP_NAME" == "" ]; then
			echo "Creating websocket.exe"
			gcc -rdynamic -o bin/websocket websocket.c -L./lib -lfunctions -ldl -lpthread
		else
			echo "Creating $COMPILE_APP_NAME"
			gcc -rdynamic -o bin/$COMPILE_APP_NAME websocket.c -L./lib -lfunctions -ldl -lpthread
		fi
		gcc -o run.exe run.c
	else
		if [ "$COMPILE_APP_NAME" == "" ]; then
			echo "Creating websocket.out"
			gcc -rdynamic -o bin/websocket.out websocket.c -L./so -lfunctions -ldl -lpthread
		else
			echo "Creating $COMPILE_APP_NAME"
			gcc -rdynamic -o bin/$COMPILE_APP_NAME websocket.c -L./so -lfunctions -ldl -lpthread
		fi
		gcc -o run.out run.c
	fi
	
	echo "App compiled successfully."
fi

#run app
if [ "$RUN_APP" == "1" ]; then

	if [ "$OS" == "Windows_NT" ]; then
		if [ "$LIBRARY_PATH" == "" ]; then
			unset LIBRARY_PATH
			export LIBRARY_PATH=$(pwd)/lib:$LIBRARY_PATH
		fi
		export PATH=$ORIGINAL_PATH:$(pwd)/lib
		
		if [ "$RUN_APP_NAME" == "" ]; then
			RUN_APP_NAME="websocket.exe"
		fi
	else
		if [ "$LD_LIBRARY_PATH" == "" ]; then
			unset LD_LIBRARY_PATH
			export LD_LIBRARY_PATH=$(pwd)/so:$LD_LIBRARY_PATH
		fi
		
		if [ "$RUN_APP_NAME" == "" ]; then
			RUN_APP_NAME="websocket.out"
		fi
	fi
	
	bin/$RUN_APP_NAME
fi

#its all over now!
echo ""
echo "Script is now finished"
