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


//CONSTANTS
#include "constants.h"


//STRUCTURES
#include "structs.h"

//PROTOTYPES
void *serverStart();
void *clientThread(void *s);

extern int getSocket(clientStruct s);
extern int getActive(clientStruct s);
extern char *getName(clientStruct s);

extern void *sendMessage(int sock, char *s, int len);

void *alterStruct(int sock, char *action);
extern pid_t execute(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err);
extern void *performAction(char *cmd, clientStruct *s);

extern void *consoleCommand();

void *printInt(void *num);

clientStruct * socketArray(int position);


//GLOBALS
//extern clientStruct temp[NUM_OF_CLIENTS];
//#include "globalVars.h"


/*
 * http://en.wikipedia.org/wiki/Standard_streams#1970s:_C_and_Unix
 * 
 * In the C programming language, the standard input, output, and error streams are
 * attached to the existing Unix file descriptors 0, 1 and 2 respectively.[1]
 * In a POSIX environment the <unistd.h> definitions STDIN_FILENO, STDOUT_FILENO or STDERR_FILENO
 * should be used instead rather than magic numbers.
 * File pointers stdin, stdout, and stderr are also provided.
 * 
 * function from http://stackoverflow.com/questions/1047385/
 */

/*
 *	printInt: the is a legacy function used when testing threads.
 *
 *	ARGUEMENTS
 *	num:	Any 32-bit integer
 */
/*
void *printInt (void *num) {
	int *x =	(int *)num;
	printf("%s: %d\n", __func__, *x);
	return NULL;
}
*/
#endif /* WEBSOCK_C */
