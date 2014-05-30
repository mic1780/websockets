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

//REQUIRED INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>


//STRUCTURES
#include "structs.h"

//PROTOTYPES

//websocket.c
extern void *serverStart();
extern void *clientThread(void *s);
extern void *consoleCommand();
extern void printCommandList();

//alterStruct.c
extern int getSocket(clientStruct s);
extern void setSocket(clientStruct * s, int val);

extern int getActive(clientStruct s);
extern void setActive(clientStruct * s, int bitFlag);

extern char *getName(clientStruct s);
extern void setName(clientStruct * s, char * name);

extern void *alterStruct(int sock, char *action);

extern void initializeSockets(clientStruct * sock);

extern clientStruct * socketArray(int position);// __attribute__((unused));

//sendMessage.c
extern void *sendMessage(int sock, char *s, int len);

//execute.c
extern pid_t execute(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err);

//performAction.c
extern void *performAction(char *cmd, clientStruct *s);
//extern void ** createHolder(int sock, char *s, int len);
extern void ** createISIHolder(int sock, char *s, int len);//int string int
extern void ** createSCSHolder(char *s, clientStruct *cli);//string clientStruct
extern void destroyHolder(void ** holder, int len);

//dlFunctions.c
extern void * doFunction(char *func, void ** args);

#endif /* WEBSOCK_C */
