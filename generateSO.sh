#!/bin/bash
for i in $*; do
#	echo "\""$i"\""
	gcc -c $i.c
	mv $i.o ./objects/$i.o
	gcc -shared -o lib$i.so ./objects/$i.o
	mv lib$i.so ./so/$i.so
done
