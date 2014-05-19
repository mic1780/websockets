#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "include/functions.h"

int checkForError(char *error) {
	if (error != NULL) {
		printf("ERROR: %s\n", error);
		exit(EXIT_FAILURE);
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
	//filename = NULL;
	if (!handle) {
		checkForError(dlerror());
	}//END IF
	
	dlerror();
	
	typedef void * (*funcType)(int, char *, int);
	functionPtr = dlsym(handle, func);
	checkForError(dlerror());
	funcType sendMessage = (funcType) functionPtr;
	(sendMessage)((int)args[0], (char *)args[1], (int)args[2]);
	dlclose(handle);
	handle = NULL;
	return NULL;
	
	if (strncmp(func, "sendMessage", strlen(func)) == 0) {
		void * (*funcPtr)(int, char *, int);
		*(void **) (&funcPtr) = dlsym(handle, func);
		//funcPtr = (void *)dlsym(handle, func);
		checkForError(dlerror());
		(*funcPtr)((int)args[0], (char *)args[1], (int)args[2]);
	}// else if (strncmp(func, "alterStruct", strlen(func)) == 0) {
		//void * (*funcPtr)(int sock, char *action);
	//} else if (strncmp(func, "execute", strlen(func)) == 0) {
		//void * (*funcPtr)(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err);
	//} else {
		//void * (*funcPtr)(int sock, char *s, int len);
	//}//END IF
	
	dlclose(handle);
	handle = NULL;
	return NULL;
	
	
	return NULL;
	
}//END loadFunction
