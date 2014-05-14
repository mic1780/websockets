#!/bin/bash
#for i in $*; do
##	echo "\""$i"\""
#	gcc -c -Wall -fPIC $i.c
#	mv $i.o ./objects/$i.o
#	gcc -shared ./objects/$i.o -o lib$i.so
#	mv lib$i.so ./so/$i.so
#done

for i in $*; do
	gcc -c -Wall -fPIC $i.c
	mv $i.o ./objects/$i.o
done
gcc -shared -o libfunctions.dll ./objects/*.o 
#mv libfunctions.dll so/libfunctions.dll
#gcc -o websocketShared websocket.c -L./ -lfunctions
