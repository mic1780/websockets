#include <stdio.h>
#include <pthread.h>

#ifndef STRUCTS
#define STRUCTS

typedef struct client {
	int sock;
	int active;
	int isAdmin;
	char *name;
	pthread_t t;
} clientStruct;

#endif
