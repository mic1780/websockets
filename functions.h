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

//functions.h

#ifndef WEBSOCK_C
#define WEBSOCK_C


//define constants
#define TRUE 1
#define FALSE 0

#define CONNECTING 0
#define OPEN 1
#define CLOSING 2
#define CLOSED 3
#define NUM_OF_CLIENTS 600


//STRUCTURES
typedef struct client {
	int sock;
	int active;
	int isAdmin;
	char *name;
	pthread_t t;
} clientStruct;


//PROTOTYPES
void *serverStart();
void *clientThread(void *s);

int getSocket(clientStruct s);
int getActive(clientStruct s);
char *getName(clientStruct s);

void *sendMessage(int sock, char *s, int len);

void *alterStruct(int sock, char *action);
pid_t execute(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err);
void *performAction(char *cmd, clientStruct *s);

void *consoleCommand();

void *printInt(void *num);


//GLOBALS
static int serv;
static clientStruct temp[NUM_OF_CLIENTS];
static char ipAddress[16];
//static int clients[NUM_OF_CLIENTS];

#endif /* WEBSOCK_C */
