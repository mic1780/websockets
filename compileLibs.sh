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
# HOW I LINKED MAIN PROGRAM TO alterStruct
#		gcc -c -o objects/libfunctions.o libfunctions.c
#		gcc -shared -o lib/libfunctions.dll objects/libfunctions.o
#		gcc -shared -o lib/libalterStruct.dll objects/alterStruct.o lib/libfunctions.dll

echo "Creating tmp directory"
mkdir tmp

for i in $*; do
	echo "Generating $i.o"
	gcc -c -Wall -o tmp/$i.o $i.c
	if [ "$i" != "sendMessage" -a "$i" != "alterStruct" -a "$i" != "performAction" ]; then
		echo "Do not compile $i here. (Use compileFunctions.sh)"
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
