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
static clientStruct temp[NUM_OF_CLIENTS];


//FUNCTIONS
/*
 * sendMessage: this function is used then we want to send message (s)
 *              of length (len) from the server to a client (sock)
 *
 * ARGUMENTS
 * sock:	the socket where we want the message to go
 * s:		A string containing the message we want to send
 * len:	the length of the string s
 */
void *sendMessage(int sock, char *s, int len) {
	int frameCount;
	uint16_t len16;
	char frame[10];
	char *reply =	malloc(sizeof(char) * (len + 8));
	
	//len = strlen(s);
	frame[0] =	'\x81';
	
	if (len <= 125) {
		frame[1] = len;
		frameCount = 2;
	} else if (len >= 126 && len <= 65535) {
		frame[1] = 126;
		len16 =	htons(len);
		memcpy(frame + 2, &len16, 2);
		frameCount = 4;
	} else {
		frame[1] = 127;
		//NOTE: HAVE NOT FULLY CONFIGURED A MESSAGE OF THIS LENGTH (TODO)
		//frame[2] = (char)( ((char)len >> 56) & (char)255 );
		//frame[3] = (char)( ((char)len >> 48) & (char)255 );
		//frame[4] = (char)( ((char)len >> 40) & (char)255 );
		//frame[5] = (char)( ((char)len >> 32) & (char)255 );
		//frame[6] = (char)( ((char)len >> 24) & (char)255 );
		frame[7] = (char)( ((char)len >> 16) & (char)255 );
		frame[8] = (char)( ((char)len >> 8) & (char)255 );
		frame[9] = (char)( ((char)len) & (char)255 );
		frameCount = 10;
	}//END IF
	
	memcpy(reply, frame, frameCount);
	memcpy(reply + frameCount, s, len);
	
	if (write(sock, reply, strlen(reply)) <= 0) {
		printf("\n\nWE ARE NOT WRITING!\n\n");
	}//END IF
	
	free(reply);
	reply = NULL;
	
	return NULL;
}//END FUNCTION


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
pid_t execute(const char *command, clientStruct s, FILE **in, FILE **out, FILE **err) {
	pid_t pid;
	char *buf;
	int pos;
	int   fd[6];
	
	pipe(&fd[0]);
	pipe(&fd[2]);
	pipe(&fd[4]);
	
	switch (pid = fork()) {
		case -1://error
			perror("unable to fork()");
			exit(1);
			
		case 0://child
			close(fd[1]);   // Close write end of stdin.
			close(fd[2]);   // Close read end of stdout.
			close(fd[4]);   // Close read end of stderr.
			
			dup2(fd[0], STDIN_FILENO);  // Have stdin read from the first pipe.
			dup2(fd[3], STDOUT_FILENO); // Have stdout write to the second pipe.
			dup2(fd[5], STDERR_FILENO); // Have stderr write to the third pipe.
			
			//NEW execlp used with execute.exe (may get rid of this)
			execlp("/bin/bash", "./execute.exe", "-c", command, NULL);
			
			//OLD execlp used with php script
			//execlp("/bin/bash", "/bin/bash", "-c", command, NULL);
			
			perror("execlp() failed");
			_exit(1);
	
		default://parent
			close(fd[0]); // Close read end of stdin.
			close(fd[3]); // Close write end of stdout.
			close(fd[5]); // Close write end of stderr.
			
			//read what comes back from stdout
			
			buf =	malloc(sizeof(char) * 100);
			while ((pos = read(fd[2], buf, 100)) > 0) {
				buf[pos] =	'\0';
				sendMessage(getSocket(s), buf, strlen(buf));
				memset(&buf, '\0', sizeof(buf));
				free(buf);
				buf =	NULL;
				buf =	malloc(sizeof(char) * 100);
			}//END WHILE LOOP
			memset(&buf, '\0', sizeof(buf));
			free(buf);
			buf = NULL;
			
			/**/
			if (in)  *in  = fdopen(fd[1], "wb"); else close(fd[1]);
			if (out) *out = fdopen(fd[2], "rb"); else close(fd[2]);//stdout ends here
			if (err) *err = fdopen(fd[4], "rb"); else close(fd[4]);
			/**/
			return pid;
	}//END SWITCH
	
}//END FUNCTION


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
	
	if (strncmp(cmd, "test", 4) == 0) {
		execute("wshome ; php exec.php test", *s, NULL, NULL, NULL);
	} else if (strncmp(cmd, "sql", 3) == 0) {
		strcpy(fullCmd, "php exec.php sql '");
		strcat(fullCmd, cmd + 4);
		strcat(fullCmd, "'");
		
		execute(fullCmd, *s, NULL, NULL, NULL);
	} else if (strncmp(cmd, "set", 3) == 0) {
		if (strncmp(cmd + 4, "name", 4) == 0) {
			s->name =	alterStruct(getSocket(*s), cmd);
		}//END IF
	} else if (strncmp(cmd, "sendall", 7) == 0) {
		sprintf(fullCmd, "Message from %s: %s", getName(*s), cmd+8);
		for (i=0; i < NUM_OF_CLIENTS; i++) {
			if (getActive(temp[i]) == 0 || getSocket(*s) == getSocket(temp[i])) {
				//do nothing
			} else {
				sendMessage(getSocket(temp[i]), fullCmd, strlen(fullCmd));
			}//END IF
		}//END FOR LOOP
	} else {
		sendMessage(getSocket(*s), cmd, strlen(cmd));
	}//END IF

	return NULL;
}//END FUNCTION


/*
 *	printInt: the is a legacy function used when testing threads.
 *
 *	ARGUEMENTS
 *	num:	Any 32-bit integer
 */
void *printInt (void *num) {
	int *x =	(int *)num;
	printf("%s: %d\n", __func__, *x);
	return NULL;
}

#endif /* WEBSOCK_C */
