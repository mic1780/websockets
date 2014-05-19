#!/bin/bash

for i in $*; do
	gcc -c -Wall -fPIC $i.c
	mv $i.o objects/$i.o
	if [ "$i" == "performAction" ]; then
		gcc -shared -o lib/lib$i.dll objects/*.o libfunctions.c
	else
		gcc -shared -o lib/lib$i.dll objects/$i.o libfunctions.c
	fi
done
gcc -shared -o libfunctions.dll objects/*.o libfunctions.c
#mv libfunctions.dll so/libfunctions.dll
#gcc -o websocketShared websocket.c -L./ -lfunctions
