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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "include/functions.h"

int checkForError(char *error) {
	if (error != NULL) {
		printf("ERROR: %s\n", error);
		return 1;
		//exit(EXIT_FAILURE);
	}//END IF
	
	return 0;
}//END IF

void * doFunction(char *func, void ** args) {
	void *handle;
	void * functionPtr;
	//void * (*alterStruct)(int sock, char *action);
	
	int filenameLength;
	char * filename = NULL;
	//void *(*funcPtr);
	
	
	filenameLength = strlen("lib/lib") + strlen(func) + strlen(".dll");
	filename = malloc(sizeof(char) * (filenameLength + 1));
	strcpy(filename, "lib/lib");
	strcat(filename, func);
	strcat(filename, ".dll");
	
	handle = dlopen(filename, RTLD_LAZY);
	memset(filename, '\0', filenameLength);
	free(filename);
	filename = NULL;
	if (!handle) {
		checkForError(dlerror());
		return NULL;
	}//END IF
	
	dlerror();
	
	if (strncmp(func, "sendMessage", strlen(func)) == 0) {
		typedef void * (*funcType)(int, char *, int);
		functionPtr = dlsym(handle, func);
		if (checkForError(dlerror()) == 0) {
			funcType sendMessage = (funcType) functionPtr;
			(sendMessage)((int)args[0], (char *)args[1], (int)args[2]);
		}//END IF
	} else if (strncmp(func, "alterStruct", strlen(func)) == 0) {
		typedef void * (*funcType)(int, char *);
		functionPtr = dlsym(handle, func);
		if (checkForError(dlerror()) == 0) {
			funcType alterStruct = (funcType) functionPtr;
			(alterStruct)((int)args[0], (char *)args[1]);//error here
		}//END IF
	}// else if (strncmp(func, "execute", strlen(func)) == 0) {
		//void * (*funcPtr)(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err);
	//} else {
		//void * (*funcPtr)(int sock, char *s, int len);
	//}//END IF
	
	dlclose(handle);
	handle = NULL;
	return NULL;
	
}//END loadFunction
