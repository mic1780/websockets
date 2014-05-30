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

if [ "$1" == "run" -o "$1" == "run.exe" -o "$1" == "run.sh" ]; then
	echo ERROR: Do not overwrite the provided run executables.
	exit
fi

if [ "$OS" == "Windows_NT" ]; then
	if [ "$1" == "" ]; then
		echo "Creating websocket.exe"
		gcc -rdynamic -o websocket websocket.c -L./lib -lfunctions -ldl -lpthread
	else
		echo "Creating $1"
		gcc -rdynamic -o $1 websocket.c -L./lib -lfunctions -ldl -lpthread
	fi
	gcc -o run.exe run.c
else
	if [ "$1" == "" ]; then
		echo "Creating websocket.out"
		gcc -rdynamic -o websocket.out websocket.c -L./so -lfunctions -ldl -lpthread
	else
		echo "Creating $1"
		gcc -rdynamic -o $1 websocket.c -L./so -lfunctions -ldl -lpthread
	fi
	gcc -o run.sh run.c
fi

echo "App compiled successfully."
