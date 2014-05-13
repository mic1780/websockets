#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/structs.h"
#include "include/constants.h"

//STRUCT HELPER FUNCTIONS

int getSocket(clientStruct s) {
	return s.sock;
}

int getActive(clientStruct s) {
	return s.active;
}

char *getName(clientStruct s) {
	return s.name;
}

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
	
	if (strcmp(action, "init") == 0) {
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			
			if (temp[i].active == FALSE) {
				break;
			}
			
		}//END FOR LOOP
		
		temp[i].active =	TRUE;
		temp[i].sock =		sock;
		
		return (int *)i;
	} else if (strcmp(action, "close") == 0) {
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			
			if (getSocket(temp[i]) == sock) {
				break;
			}
			
		}//END FOR LOOP
		close(getSocket(temp[i]));
		
		free(temp[i].name);
		temp[i].name =	NULL;
		
		temp[i].active =	FALSE;
		temp[i].sock =	0;
		
		return NULL;
	} else if (strncmp(action, "set", 3) == 0) {
		
		/* What we can set:
		 * name
		 * isAdmin
		 */
		if (strncmp(action + 4, "name", 4) == 0) {
			for (i=0; i < NUM_OF_CLIENTS; i++) {
				if (getSocket(temp[i]) == sock) {
					
					break;
					
				}//END IF
			}//END FOR LOOP
			
			temp[i].name =	malloc(sizeof(char) * (strlen(action + 9) + 1));
			strcpy(temp[i].name, action + 9);
			return getName(temp[i]);
		}//END IF
		
		return NULL;
	}//END IF
	
	printf("\n\nWe were able to alter struct\n\n");
	printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, temp[i].active, temp[i].sock);
	return NULL;
}//END FUNCTION
