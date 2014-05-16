#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
#include <sys/socket.h>
#include "include/structs.h"
#include "include/constants.h"
#include "include/functions.h"
//#include "include/globalVars.h"

clientStruct * socketArray(int position) {
	static clientStruct temp[NUM_OF_CLIENTS];
	static int initialized = 0;
	
	if (initialized == 0) {
		printf("\n\n\t\t **** INITIALIZING temp ****\n\n");
		initialized++;
		initializeSockets(temp);
	}
	return &temp[position];
}
//STRUCT HELPER FUNCTIONS

void initializeSockets(clientStruct * sock) {
	int i;
	for (i=0; i < NUM_OF_CLIENTS; i++) {
		setSocket(&sock[i], 0);
		setActive(&sock[i], FALSE);
		sock[i].name = NULL;
	}//END FOR LOOP
}//END FUNCTION

/*

int getSocket(clientStruct s) {
	return s.sock;
}
void setSocket(clientStruct * s, int val) {
	s->sock = val;
}

int getActive(clientStruct s) {
	return s.active;
}
void setActive(clientStruct * s, int bitFlag) {
	s->active = bitFlag;
}

char *getName(clientStruct s) {
	return s.name;
}
void setName(clientStruct * s, char * name) {
	if (name == NULL && s->name != NULL) {
		free(s->name);
		s->name = NULL;
	} else if (name != NULL) {
		if (s->name != NULL) {
			setName(s, NULL);
		}//END IF
		s->name =	malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(s->name, name);
	}//END IF
}
*/

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
			
			if (getActive(*socketArray(i)) == FALSE) {
				break;
			}
			
		}//END FOR LOOP
		
		setActive(socketArray(i), TRUE);
		//temp[i].active =	TRUE;
		setSocket(socketArray(i), sock);
		//temp[i].sock =		sock;
		
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
		
		shutdown(getSocket(*socketArray(i)), 2);
		close(getSocket(*socketArray(i)));
		
		setName(socketArray(i), NULL);
		//free(temp[i].name);
		//temp[i].name =	NULL;
		
		setActive(socketArray(i), FALSE);
		//temp[i].active =	FALSE;
		
		setSocket(socketArray(i), 0);
		//temp[i].sock =	0;
		
		return NULL;
	} else if (strncmp(action, "set", 3) == 0) {
		
		/* What we can set:
		 * name
		 * isAdmin
		 */
		if (strncmp(action + 4, "name", 4) == 0) {
			for (i=0; i < NUM_OF_CLIENTS; i++) {
				if (getSocket(*socketArray(i)) == sock) {
					
					break;
					
				}//END IF
			}//END FOR LOOP
			
			setName(socketArray(i), action + 9);
			//temp[i].name =	malloc(sizeof(char) * (strlen(action + 9) + 1));
			//strcpy(temp[i].name, action + 9);
			return getName(*socketArray(i));
		}//END IF
		
		return NULL;
	}//END IF
	
	printf("\n\nWe were able to alter struct\n\n");
	printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, getActive(*socketArray(i)), getSocket(*socketArray(i)));
	//printf("temp[%d]\n\tactive = %d\n\tsock = %d\n\n", i, temp[i].active, temp[i].sock);
	return NULL;
}//END FUNCTION
