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

for i in $*; do
	if [ "$i" == "sendMessage" -o "$i" == "alterStruct" -o "$i" == "performAction" ]; then
		echo "Do not compile $i here. (Use compileLibs.sh)"
	else
		echo "Generating $i.o"
		gcc -c -Wall -o objects/$i.o $i.c
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
