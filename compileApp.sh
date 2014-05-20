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

if [ "$1" == "" ]; then
	gcc -rdynamic -o websocket websocket.c -L. -lfunctions -ldl
else
	gcc -rdynamic -o $1 websocket.c libfunctions.c -L. -lfunctions -ldl
fi
echo App compiled successfully.
