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
#include "../include/functions.h"
#include "../include/constants.h"

int checkForError(char *error) {
	if (error != NULL) {
		printf("ERROR: %s\n", error);
		return 1;
		//exit(EXIT_FAILURE);
	}//END IF
	
	return 0;
}//END IF

void * callFunction(char *fName, void ** argv) {
	void *handle;
	void * functionPtr;
	void * returnVal;
	
	int filenameLength;
	char * filename = NULL;
	
	if (__ISWINDOWS__) {
		filenameLength = strlen("lib/lib") + strlen(fName) + strlen(".dll");
	} else {
		filenameLength = strlen("so/lib") + strlen(fName) + strlen(".so");
	}//END IF
	
	filename = malloc(sizeof(char) * (filenameLength + 1));
	
	if (__ISWINDOWS__) {
		strcpy(filename, "lib/lib");
		strcat(filename, fName);
		strcat(filename, ".dll");
	} else {
		strcpy(filename, "so/lib");
		strcat(filename, fName);
		strcat(filename, ".so");
	}//END IF
	
	handle = dlopen(filename, RTLD_LAZY);
	memset(filename, '\0', filenameLength);
	free(filename);
	filename = NULL;
	if (!handle) {
		checkForError(dlerror());
		return NULL;
	}//END IF
	
	dlerror();
	
	if (strncmp(fName, "sendMessage", strlen(fName)) == 0) {
		typedef void * (*funcType)(int, char *, int);
		functionPtr = dlsym(handle, fName);
		if (checkForError(dlerror()) == 0) {
			funcType sendMessage = (funcType) functionPtr;
			returnVal = (sendMessage)((int)argv[0], (char *)argv[1], (int)argv[2]);
		}//END IF
	} else if (strncmp(fName, "alterStruct", strlen(fName)) == 0) {
		typedef void * (*funcType)(int, char *);
		functionPtr = dlsym(handle, fName);
		if (checkForError(dlerror()) == 0) {
			funcType alterStruct = (funcType) functionPtr;
			
			if (strcmp((char *)argv[1], "init") == 0 || strcmp((char *)argv[1], "close") == 0) {
				returnVal = (int *)((alterStruct)((int)argv[0], (char *)argv[1]));
			} else {
				returnVal = (char *)((alterStruct)((int)argv[0], (char *)argv[1]));//error here
			}//END IF
			
		}//END IF
	} else if (strncmp(fName, "performAction", strlen(fName)) == 0) {
		typedef void * (*funcType)(char *, clientStruct *);
		functionPtr = dlsym(handle, fName);
		if (checkForError(dlerror()) == 0) {
			funcType performAction = (funcType) functionPtr;
			returnVal = (performAction)((char *)argv[0], (clientStruct *)argv[1]);
		}//END IF
	}// else {
		//void * (*funcPtr)(int sock, char *s, int len);
	//}//END IF
	
	dlclose(handle);
	handle = NULL;
	return returnVal;
	
}//END loadFunction
