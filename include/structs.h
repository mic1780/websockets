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
#include <pthread.h>

#ifndef STRUCTS
#define STRUCTS

typedef struct server {
	char * name;
	char * path;
	char ip[16];
	int port;
	int isRunning;
	pthread_t t;
} serverStruct;

//the main structure that the program is based around
typedef struct client {
	int sock;
	int active;
	int isAdmin;
	int isMonitor;
	char *name;
	pthread_t t;
} clientStruct;

//node structure for a potential change to linked list instead of static array
typedef struct clientNode {
	clientStruct client;
	struct clientNode * next;
	struct clientNode * prev;
} clientNode;

typedef struct module {
	struct module * next;
	char * name;
	char * path;
	char ip[16];
	int port;
} module;

#endif
