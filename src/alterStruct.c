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
#include <sys/socket.h>
#include "../include/structs.h"
#include "../include/constants.h"
#include "../include/functions.h"

/*
 * alterStruct: this function is used when we want to change a value inside
 *              of our socket structure
 *
 * ARGUMENTS
 * sock:		The socket that we are looking to alter
 * action:	A string of varying length that has commands we will use
 *				to alter our structure
 *
 * LIST OF POSSIBLE ACTIONS:
 *	init -	used to initialize our socket structure. used when a socket first connects
 *
 *	close -	used when a socket has requested to be closed.
 *
 *	set -		used when we are changing a value in the structure
 *		name -used when we are changing the name inside the structure
 *			<value>
 *
 */
void *alterStruct(int sock, char *action) {
	int i = 0;
	
	//printf("sock: \"%d\"\taction: \"%s\"\n", sock, action);
	
	if (strcmp(action, "init") == 0) {
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			
			if (getActive(*socketArray(i)) == FALSE) {
				break;
			}
			
		}//END FOR LOOP
		
		setActive(socketArray(i), TRUE);
		setSocket(socketArray(i), sock);
		
		return (int *)i;
	} else if (strcmp(action, "close") == 0) {
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			
			if (getSocket(*socketArray(i)) == sock) {
				break;
			}
			
		}//END FOR LOOP
		
		if (i == NUM_OF_CLIENTS) {
			return (int *)-1;
		}//END IF
		
		if (monitorList(sock, 1, FALSE) != NULL) {
			monitorList(sock, 4, FALSE);
		}//END IF
		
		shutdown(getSocket(*socketArray(i)), 2);
		close(getSocket(*socketArray(i)));
		
		setName(socketArray(i), NULL);
		
		setActive(socketArray(i), FALSE);
		
		setSocket(socketArray(i), 0);
		
		return NULL;
	} else if (strncmp(action, "set", 3) == 0) {
		/* What we can set:
		 * name
		 * isAdmin
		 */
		
		//get the index of the socket requesting the action
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			if (getSocket(*socketArray(i)) == sock) {
				
				break;
				
			}//END IF
		}//END FOR LOOP
		if (strncmp(action + 4, "name", 4) == 0) {
			setName(socketArray(i), action + 9);
			return getName(*socketArray(i));
		} else if (strncmp(action + 4, "monitor", 7) == 0) {
			if (strlen(action) == 13 && action[12] == '1') {
				setMonitor(socketArray(i), TRUE);
				monitorList(getSocket(*socketArray(i)), 1 | 2, FALSE);
			} else if (strlen(action) == 13 && action[12] == '0') {
				setMonitor(socketArray(i), FALSE);
				monitorList(sock, 4, FALSE);
			}//END IF
			return (int *)getMonitor(*socketArray(i));
		}//END IF
		
		return NULL;
	}//END IF
	
	printf("\n\nWe were able to alter struct\n\n");
	printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, getActive(*socketArray(i)), getSocket(*socketArray(i)));
	//printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, temp[i].active, temp[i].sock);
	return NULL;
}//END FUNCTION