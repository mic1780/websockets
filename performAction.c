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
#include "include/structs.h"
#include "include/constants.h"
#include "include/functions.h"

void ** createHolder(int sock, char *s, int len);
void destroyHolder(void ** holder);

/*
 *	performAction: this function will be used when we want to do
 *						do anything with our socket.
 *
 *	ARGUMENTS
 *	cmd:	A string of varying length that holds actions to perform (space seperated)
 *	s:		the socket structure we want to perform our cammands on
 */
void *performAction(char *cmd, clientStruct *s) {
	int i;
	char fullCmd[1024];
	void ** holder;
	
	if (strncmp(cmd, "test", 4) == 0) {
		//execute("wshome ; php exec.php test", *s, NULL, NULL, NULL);
	} else if (strncmp(cmd, "sql", 3) == 0) {
		strcpy(fullCmd, "php exec.php sql '");
		strcat(fullCmd, cmd + 4);
		strcat(fullCmd, "'");
		
		//execute(fullCmd, *s, NULL, NULL, NULL);
	} else if (strncmp(cmd, "set", 3) == 0) {
		if (strncmp(cmd + 4, "name", 4) == 0) {
			s->name =	alterStruct(getSocket(*s), cmd);
		}//END IF
	} else if (strncmp(cmd, "sendall", 7) == 0) {
		sprintf(fullCmd, "Message from %s: %s", getName(*s), cmd+8);
		holder = createHolder(getSocket(*s), fullCmd, strlen(fullCmd));
		doFunction("sendMessage", holder);
		destroyHolder(holder);
		
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			if (getActive(*socketArray(i)) == 0 || getSocket(*s) == getSocket(*socketArray(i))) {
				//do nothing
			} else {
				holder = createHolder(getSocket(*socketArray(i)), fullCmd, strlen(fullCmd));
				doFunction("sendMessage", holder);
				destroyHolder(holder);
			}//END IF
		}//END FOR LOOP
	} else {
		holder = createHolder(getSocket(*s), cmd, strlen(cmd));
		doFunction("sendMessage", holder);
		destroyHolder(holder);
	}//END IF

	return NULL;
}//END FUNCTION

void ** createHolder(int sock, char *s, int len) {
	void ** holder;
	holder = malloc(sizeof(void *) * 3);
	holder[0] = (int *)sock;
	holder[1] = (char *)s;
	holder[2] = (int *)len;
	return holder;
}

void destroyHolder(void ** holder) {
	holder[0] = NULL;
	holder[1] = NULL;
	holder[2] = NULL;
	free(holder);
	holder = NULL;
}
