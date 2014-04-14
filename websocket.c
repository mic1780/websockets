/*
 *	@file: websocket.c
 * @author: Michael Cummins
 * @date: 4/11/2013 12:34PM
 * @compiler:	GCC
 * @32-bit: YES
 * @description:	This program implements a C version of websockets.
 *						When it starts it uses the internal IP of host and specified port
 *						to server websocket requests from client computers.
 *
 *	Websocket info
 * @supportedVersions:	13
 * @numberOfClients:		Dynamic
 *	@clientsInside:		static array
 */

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include "sha1.h"
#include "base64.h"

//DEFINES
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
//static int clients[NUM_OF_CLIENTS];

//FUNCTIONS
int main(void) {
	int x = 0;
	char cmd[1024];
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, consoleCommand, NULL);
	serverStart();
	//pthread_create(&serverThread, NULL, serverStart, NULL);
	//pthread_join(serverThread, NULL);
	/**
	while (TRUE) {
		printf("Please enter a command.\n");
		scanf("%d", &x);
		//printf("Initiating shutdown on socket %d\n", x);
		shutdown(x, 2);
		memset(&cmd, '\0', sizeof(cmd));
	}//END WHILE LOOP
	/**/
	//printf("Doing Stuff Here\n\nDoing More Stuff Here\n");
	//printf("Doing Stuff Here\n\nDoing More Stuff Here\n");
	return 0;
}

void *consoleCommand() {
	long x;
	char cmd[100];
	while (TRUE) {
		printf("Please enter a command.\n");
		read(STDIN_FILENO, cmd, sizeof(cmd));
		x =	strtol(cmd, NULL, 10);
		shutdown(x, 2);
	}//END WHILE LOOP
	return NULL;
}//END FUNCTION

void *serverStart() {
	int i;
	int s;
	int rc;
	struct sockaddr_in serverInfo;
	struct sockaddr clientInfo;
	int clientSocket, clientInfoLen;
	pthread_t thread1;
	
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr =	inet_addr("127.0.0.1");//192.168.1.100
	serverInfo.sin_port =	htons(81);
	
	s =	socket(AF_INET, SOCK_STREAM, 0);
	rc =	bind(s, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
	listen(s, 10);
	printf("Server Started.\n");
	
	clientInfoLen =	sizeof(clientInfo);
	while (TRUE) {
		clientSocket =	accept(s, &clientInfo, &clientInfoLen);
		i = (int)alterStruct(clientSocket, "init");
		printf("Client %d connected\n", clientSocket);
		pthread_create(&(temp[i].t), NULL, clientThread, &temp[i]);
		//temp[i].t =	thread1;
	}//END WHILE LOOP
	/**
	while (TRUE) {
		pthread_create(&thread1, NULL, printInt, &x);
		pthread_join(thread1, NULL);
	}//END WHILE LOOP
	/**/
	return NULL;
}

void *clientThread (void *s) {
	int i, j, pos, maskIndex;
	
	clientStruct cli =	*(clientStruct *)s;
	
	char currHeader[100];//our current header
	char currValue[512];//our value for our current header
	char shaOutput[40];
	
	char key[100];//the initial key from headers
	unsigned char midKey[20];
	char endKey[200];//key after append and SHA1
	
	SHA1Context sha;
	base64_encodestate b64_ctx;
	
	char readBuffer[1024], writeBuffer[1024], masks[4];//the buffers
	int bytes;//total received bytes.
	int flag = 0;//header or value: 0 = header, 1 = value
	
	printf("New thread created\n");
	bytes =	read(getSocket(cli), readBuffer, sizeof(readBuffer)-1);
	//printf("Connection received. Parsing headers.\n");
	for (i=0; i < bytes; i++) {
		if (flag == 0) {
			if (readBuffer[i] == '\r') {
				i++;
				memset(&currHeader, '\0', sizeof(currHeader)-1);
			} else if (currHeader[0] == '\0') {
				currHeader[0] = readBuffer[i];
			} else if (readBuffer[i] == ':') {
				i++;
				flag = 1;
			} else {
				currHeader[strlen(currHeader)] = readBuffer[i];
			}//END IF
		} else {
			if (currValue[0] == '\0') {
				currValue[0] = readBuffer[i];
			} else if (readBuffer[i] == '\r') {
				if (strcmp(currHeader, "Sec-WebSocket-Key") == 0) {
					SHA1Reset(&sha);
					strcpy(key, currValue);
					strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					SHA1Input(&sha, key, strlen(key));
					SHA1Result(&sha);
					memset(&key, '\0', strlen(key) - 1);
					for(j = 0; j < 5; ++j) {	// put in correct byte order before memcpy.
						sha.Message_Digest[j] = ntohl(sha.Message_Digest[j]);
					}//END FOR LOOP
					memcpy(midKey, (unsigned char*)sha.Message_Digest, 20);
					
					// encode `midKey' in base 64, into `endKey'.
					base64_init_encodestate(&b64_ctx);
					pos = base64_encode_block((const char*)midKey, 20, endKey, &b64_ctx);
					base64_encode_blockend(endKey + pos, &b64_ctx);
					
				}//END IF
				//printf("%s: %s\n", currHeader, currValue);
				memset(&currHeader, '\0', sizeof(currHeader));
				memset(&currValue, '\0', sizeof(currValue));
				i++;
				flag = 0;
			} else {
				currValue[strlen(currValue)] = readBuffer[i];
			}//END IF
		}//END IF
	}//END FOR LOOP
	//printf("Headers parsed. Sending back response.\n");
	strcpy(writeBuffer, "HTTP/1.1 101 Switching Protocols\r\n");
	strcat(writeBuffer, "Upgrade: websocket\r\n");
	strcat(writeBuffer, "Connection: Upgrade\r\n");
	//write the sec-ws-accept to buffer
	strcat(writeBuffer, "Sec-WebSocket-Accept: ");
	strcat(writeBuffer, endKey);
	strcat(writeBuffer, "\r\n");
	write(getSocket(cli), writeBuffer, strlen(writeBuffer));
	
	memset(&readBuffer, '\0', sizeof(readBuffer));
	
	while(TRUE) {
		bytes =	read(getSocket(cli), readBuffer, sizeof(readBuffer));
		
		//If the byteStream was closed or we receive a close byte, confirm close and release connection
		if (bytes <= 0 || readBuffer[0] == '\x88') {
			printf("Client #%d Closed.\n", getSocket(cli));
			alterStruct(getSocket(cli), "close");
			break;
		} else if (bytes > 0) {
			
			//first find the mask index
			pos = atoi(&readBuffer[1]);
			maskIndex = 2;
			if ((readBuffer[1] & 0x7f) == 126) {
				maskIndex = 4;
			} else if ((readBuffer[1] & 0x7f) == 127) {
				maskIndex = 10;
			}//END IF
			
			//reset buffer to NULL bytes
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
			
			//grab the mask bytes from the message
			masks[0] = readBuffer[maskIndex];
			masks[1] = readBuffer[maskIndex + 1];
			masks[2] = readBuffer[maskIndex + 2];
			masks[3] = readBuffer[maskIndex + 3];
			maskIndex += 4;
			
			//next use the maskIndex to rewrite the existing bytes to decode the message
			for(j=maskIndex; j < bytes; j++) {
				writeBuffer[j - maskIndex] = readBuffer[j] ^ masks[(j - maskIndex) % 4];
			}//END FOR LOOP
			
			printf("Message from socket #%d: \"%s\"\n", getSocket(cli), writeBuffer);
			//printf("Bytes: %d\tmaskIndex: %d\n", bytes, maskIndex);
			
			//send the decoded message to the performAction function
			performAction(writeBuffer, &cli);
			
			//reset buffer to NULL bytes
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
		}//END IF
	}//END WHILE LOOP
	
	printf("Thread ended.\n");
	
	return NULL;
}//END FUNCTION

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
	int i, frameCount;
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
	//reply =	malloc(sizeof(char) * (frameCount + len));
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
	int i;
	
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
			
			execlp("/bin/bash", "/bin/bash", "-c", command, NULL);
			//execlp("/bin/bash", "/bin/bash", "-c", command, (char *)NULL);//command
			
			perror("execlp() failed");
			_exit(1);
	
		default://parent
			close(fd[0]); // Close read end of stdin.
			close(fd[3]); // Close write end of stdout.
			close(fd[5]); // Close write end of stderr.
			
			//read what comes back from stdout
			//memset(&buf, 0, sizeof(buf));
			buf =	malloc(sizeof(char) * 100);
			while ((pos = read(fd[2], buf, 100)) > 0) {
				buf[pos] =	'\0';
				//printf("%s", buf);
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
