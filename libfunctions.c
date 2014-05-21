/*
 *
 *   Copyright (C) 2014  Michael Cummins
 *   License: GNUv2
 *   
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */
//NO RELATION TO libfunctions.dll
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/structs.h"
#include "include/constants.h"

#ifndef LIBFUNCS__
#define LIBFUNCS__
int getSocket(clientStruct s) {
	return s.sock;
}
void setSocket(clientStruct * s, int val) {
	//printf("we got in here\n");
	s->sock = val;
}

int getActive(clientStruct s) {
	return s.active;
}
void setActive(clientStruct * s, int bitFlag) {
	s->active = bitFlag;
}

char *getName(clientStruct s) {
	return s.name;
}
void setName(clientStruct * s, char * name) {
	if (name == NULL && s->name != NULL) {
		free(s->name);
		s->name = NULL;
	} else if (name != NULL) {
		if (s->name != NULL) {
			setName(s, NULL);
		}//END IF
		s->name =	malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(s->name, name);
	}//END IF
}

void initializeSockets(clientStruct * sock) {
	int i;
	for (i=0; i < NUM_OF_CLIENTS; i++) {
		setSocket(&sock[i], 0);
		setActive(&sock[i], FALSE);
		sock[i].name = NULL;
	}//END FOR LOOP
}//END FUNCTION

clientStruct * socketArray(int position) {
	static clientStruct temp[NUM_OF_CLIENTS];
	static int initialized = 0;
	
	if (initialized == 0) {
		printf("\n\n\t\t **** INITIALIZING temp ****\n\n");
		initialized++;
		initializeSockets(temp);
	}
	return &temp[position];
}
//STRUCT HELPER FUNCTIONS

void ** createHolder(int sock, char *s, int len) {
	void ** holder;
	holder = malloc(sizeof(void *) * 3);
	holder[0] = (int *)sock;
	holder[1] = (char *)s;
	holder[2] = (int *)len;
	return holder;
}

void ** createISIHolder(int sock, char *s, int len) {
	void ** holder;
	holder = malloc(sizeof(void *) * 3);
	holder[0] = (int *)sock;
	holder[1] = (char *)s;
	holder[2] = (int *)len;
	return holder;
}

void ** createSCSHolder(char *s, clientStruct *cli) {
	void ** holder;
	holder = malloc(sizeof(void *) * 2);
	holder[0] = (char *)s;
	holder[1] = (clientStruct *)cli;
	return holder;
}

void destroyHolder(void ** holder, int len) {
	int i;
	for (i=0; i < len; i++) {
		holder[i] = NULL;
	}//END FOR LOOP
	free(holder);
	holder = NULL;
}
#endif
