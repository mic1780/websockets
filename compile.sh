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

shopt -s expand_aliases
alias gcc='gcc -w'

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
   -e [ file1 [ file2 ... ] ]       Compile executables (not main application)
   -f [ file1 [ file2 ... ] ]       Compile functions
   -l [ file1 [ file2 ... ] ]       Compile dynamic libraries
   -r [ <name> ]                    Runs the specified program after setup
   -w                               Show warnings from GCC
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

FILE_EXTENSION=
LIB_FOLDER=
LIB_EXTENSION=
if [ "$OS" == "Windows_NT" ]; then
	FILE_EXTENSION="exe"
	LIB_FOLDER="lib"
	LIB_EXTENSION="dll"
else
	FILE_EXTENSION="out"
	LIB_FOLDER="so"
	LIB_EXTENSION="so"
fi
DEFAULT_FUNCTION_NAMES=("libfunctions")
DEFAULT_LIB_NAMES=("sendMessage" "alterStruct" "performAction" "callFunction")
DEFAULT_EXEC_NAMES=("execSQL" "getODBCDrivers" "getODBCDataSources");

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
			COMPILE_EXEC=1
			COMPILE_APP=1
			;;
		-d)
			DELETE_FILES=1
			shift
			;;
		-e)
			COMPILE_EXEC=1
			COMPILE_EXEC_NAMES=()
			while true; do
				shift
				if [[ $1 =~ ^-. ]]; then
					break
				elif [ "$1" == "" ]; then
					break
				else
					COMPILE_EXEC_NAMES[${#COMPILE_EXEC_NAMES[@]}]="$1"
				fi
			done
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
		-w)
			SHOW_WARNINGS=1
			shift
			unalias gcc
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
	
	rm -rf ./$LIB_FOLDER/*.$LIB_EXTENSION
	rm -rf bin/*.$FILE_EXTENSION
	rm -f ./*.$FILE_EXTENSION
	rm -rf objects/*.o
	rm -rf ./tmp
fi


if [ "$COMPILE_FUNCTION" == "1" -a "${#COMPILE_FUNCTION_NAMES[@]}" == "0" ]; then
	COMPILE_FUNCTION_NAMES=${DEFAULT_FUNCTION_NAMES[@]}
fi

if [ "$COMPILE_LIB" == "1" -a "${#COMPILE_LIB_NAMES[@]}" == "0" ]; then
	COMPILE_LIB_NAMES=${DEFAULT_LIB_NAMES[@]}
fi

if [ "$COMPILE_EXEC" == "1" -a "${#COMPILE_EXEC_NAMES[@]}" == "0" ]; then
	COMPILE_EXEC_NAMES=${DEFAULT_EXEC_NAMES[@]}
fi


#compile functions
if [ "$COMPILE_FUNCTION" == "1" ]; then
	
	for i in ${COMPILE_FUNCTION_NAMES[@]}; do
		if [ $(contains "`echo ${DEFAULT_LIB_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -l $i)"
		elif [ $(contains "`echo ${DEFAULT_EXEC_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -e $i)"
		else
			echo "Generating $i.o"
			gcc -c -o objects/$i.o $i.c
		fi
	done

	echo "Generating libfunctions.$LIB_EXTENSION"
	gcc -shared -o $LIB_FOLDER/libfunctions.$LIB_EXTENSION objects/*.o
	
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
		elif [ $(contains "`echo ${DEFAULT_EXEC_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -e $i)"
		else
			echo "Generating $i.$LIB_EXTENSION"
			gcc -shared -o $LIB_FOLDER/lib$i.$LIB_EXTENSION tmp/$i.o $LIB_FOLDER/libfunctions.$LIB_EXTENSION
		fi
	done
	
	echo "Cleaning up..."
	rm -r tmp
	echo "Finished"
	echo ""
fi

#compile executables
if [ "$COMPILE_EXEC" == "1" ]; then
	
	for i in ${COMPILE_EXEC_NAMES[@]}; do
		if [ $(contains "`echo ${DEFAULT_FUNCTION_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -f $i)"
		elif [ $(contains "`echo ${DEFAULT_LIB_NAMES[@]}`" "$i") == "1" ]; then
			echo "Do not compile $i here. (Use ./compile.sh -l $i)"
		else
			echo "Generating $i.$FILE_EXTENSION"
			if [[ "$i" == *(ODBC|SQL)* ]]; then
				if [[ "`find /usr/include -iname 'sql.h'`" =~ "sql.h" ]]; then
					INC_LIB="-lodbc32"
				else
					echo "Can not compile $i because you do not have the required includes (sql.h)"
					continue
				fi
			else
				INC_LIB=
			fi
			gcc -o bin/$i.$FILE_EXTENSION src/$i.c $INC_LIB
		fi
	done
	
fi

#compile main application
if [ "$COMPILE_APP" == "1" ]; then
	
	if [ "$COMPILE_APP_NAME" == "" ]; then
		echo "Creating websocket.$FILE_EXTENSION"
		gcc -rdynamic -o bin/websocket.$FILE_EXTENSION websocket.c -L./$LIB_FOLDER -lfunctions -ldl -lpthread
	else
		echo "Creating $COMPILE_APP_NAME"
		gcc -rdynamic -o bin/$COMPILE_APP_NAME websocket.c -L./$LIB_FOLDER -lfunctions -ldl -lpthread
	fi
	
	gcc -o run.$FILE_EXTENSION run.c
	
	echo "App compiled successfully."
fi

#run app
if [ "$RUN_APP" == "1" ]; then

	if [ "$OS" == "Windows_NT" ]; then
		if [ "$LIBRARY_PATH" == "" ]; then
			unset LIBRARY_PATH
			export LIBRARY_PATH=$(pwd)/$LIB_FOLDER:$LIBRARY_PATH
		fi
		export PATH=$ORIGINAL_PATH:$(pwd)/$LIB_FOLDER
		
	else
		if [ "$LD_LIBRARY_PATH" == "" ]; then
			unset LD_LIBRARY_PATH
			export LD_LIBRARY_PATH=$(pwd)/$LIB_FOLDER:$LD_LIBRARY_PATH
		fi
		
	fi
	
	if [ "$RUN_APP_NAME" == "" ]; then
		RUN_APP_NAME="websocket.$FILE_EXTENSION"
	fi
	bin/$RUN_APP_NAME
fi

if [ "$SHOW_WARNINGS" == "" ]; then
	unalias gcc
fi

#its all over now!
echo ""
echo "Script is now finished"
