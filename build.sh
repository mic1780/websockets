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

echo "Beginning full compile"
echo ""

echo "Removing old files"
rm -rf lib/*.dll
rm -rf objects/*.o
rm -rf so/*.so
rm -rf ./tmp
rm -f ./*.exe
rm -f ./*.out
rm -f ./run.sh

echo ""
echo "Running compileFunctions.sh"
./compileFunctions.sh libfunctions doFunction

echo ""
echo "Running compileLibs.sh"
./compileLibs.sh sendMessage alterStruct performAction

echo ""
echo "Running compileApp.sh"
./compileApp.sh

echo ""
echo "Finished full compile"
