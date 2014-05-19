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
#!/bin/bash

mkdir tmp
for i in $*; do
	gcc -o tmp/$i.o -c -Wall -fPIC $i.c
	if [ "$i" == "performAction" ]; then
		gcc -shared -o lib/lib$i.dll tmp/*.o libfunctions.c
	else
		gcc -shared -o lib/lib$i.dll libfunctions.c tmp/$i.o
	fi
done
rm -r tmp
echo Finished compiling libraries.
