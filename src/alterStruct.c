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
	clientNode * node = NULL;
	
	//printf("sock: \"%d\"\taction: \"%s\"\n", sock, action);
	
	if (strcmp(action, "init") == 0) {
		
		//create our node in our list
		socketArray(sock, 1 | 2, FALSE);
		//setActive(getClient(socketArray(i)), TRUE);
		//setSocket(socketArray(i), sock);
		
		return (int *)i;
	} else if (strcmp(action, "close") == 0) {
		node =	socketArray(sock, 1, FALSE);
		
		if (node == NULL) {
			return (int *)-1;
		}//END IF
		
		if (monitorList(sock, 1, FALSE) != NULL) {
			monitorList(sock, 4, FALSE);
		}//END IF
		
		shutdown(getSocket(*getClient(node)), 2);
		close(getSocket(*getClient(node)));
		
		setName(getClient(node), NULL);
		
		setActive(getClient(node), FALSE);
		
		setSocket(getClient(node), 0);
		
		return NULL;
	} else if (strncmp(action, "set", 3) == 0) {
		/* What we can set:
		 * name
		 * isAdmin
		 */
		node =	socketArray(sock, 1, FALSE);
		if (node == NULL)
			return NULL;
		
		if (strncmp(action + 4, "name", 4) == 0) {
			setName(getClient(node), action + 9);
			return getName(*getClient(node));
		} else if (strncmp(action + 4, "monitor", 7) == 0) {
			if (strlen(action) == 13 && action[12] == '1') {
				setMonitor(getClient(node), TRUE);
				monitorList(getSocket(*getClient(node)), 1 | 2, FALSE);
			} else if (strlen(action) == 13 && action[12] == '0') {
				setMonitor(getClient(node), FALSE);
				monitorList(sock, 4, FALSE);
			}//END IF
			return (int *)getMonitor(*getClient(node));
		}//END IF
		
		return NULL;
	}//END IF
	
	printf("\n\nWe were able to alter struct\n\n");
	//printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, getActive(*socketArray(i)), getSocket(*socketArray(i)));
	//printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, temp[i].active, temp[i].sock);
	return NULL;
}//END FUNCTION