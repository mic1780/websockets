#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/structs.h"
#include "include/constants.h"
#include "include/functions.h"
//#include "include/globalVars.h"

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
			setName(s, cmd);
			//s->name =	alterStruct(getSocket(*s), cmd);
		}//END IF
	} else if (strncmp(cmd, "sendall", 7) == 0) {
		sprintf(fullCmd, "Message from %s: %s", getName(*s), cmd+8);
		sendMessage(getSocket(*s), fullCmd, strlen(fullCmd));
		
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			if (getActive(*socketArray(i)) == 0 || getSocket(*s) == getSocket(*socketArray(i))) {
			//if (getActive(temp[i]) == 0 || getSocket(*s) == getSocket(temp[i])) {
				//do nothing
			} else {
				sendMessage(getSocket(*socketArray(i)), fullCmd, strlen(fullCmd));
				//sendMessage(getSocket(temp[i]), fullCmd, strlen(fullCmd));
			}//END IF
		}//END FOR LOOP
	} else {
		//holder = malloc(sizeof(void *) * 3);
		//holder[0] = (int *)getSocket(*s);
		//holder[1] = (char *)cmd;
		//holder[2] = (int *)strlen(cmd);
		holder = createHolder(getSocket(*s), cmd, strlen(cmd));
		//sendMessage(getSocket(*s), cmd, strlen(cmd));
		loadFunction("sendMessage", holder);
		destroyHolder(holder);
		//free(holder);
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
	free(holder);
	holder = NULL;
}
