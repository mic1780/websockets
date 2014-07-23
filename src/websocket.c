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
/*
 *	@file: websocket.c
 * @author: Michael Cummins
 * @date: 5/19/2014 3:52PM EST
 * @compiler:	GCC
 * @32-bit: YES
 * @description:	This program implements a C version of websockets.
 *                When it starts it uses the internal IP of host and specified port
 *                to server websocket requests from client computers.
 *
 *	Websocket info
 * @supportedVersions:	13 (more later?)
 * @numberOfClients:		Dynamic (changable in "include/constants.h")
 *	@clientsInside:		socketArray function (found in libfunctions.c)
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
#include "../include/sha1.h"
#include "../include/base64.h"
#include "../include/constants.h"
#include "../include/structs.h"
#include "../include/functions.h"

//GLOBALS
static char ipAddress[16];

int listActiveSockets() {
	int i, j;
	clientStruct client;
	j=0;
	printf("\t* List of active sockets *\n\n");
	for (i=0; i < NUM_OF_CLIENTS; i++) {
		client = *socketArray(i);
		if (getActive(client) == TRUE) {
			printf("%d: Socket #%03d (%s)%c", i, getSocket(client), getName(client), (j % 2 == 0 ? '\t' : '\n'));
			j++;
		}//END IF
	}//END FOR
	if (j == 0)
		printf("(no active sockets)");
	if (j % 2 == 0)
		printf("\n");
	
	printf("\n\t* End of list *\n");
	return j;
}//END FUNCTION

void printCommandList() {
	printf("\n");
	printf("\t**** Console Command list ****\n");
	printf("exit - End this program.\n");
	printf("help - Displays this command list\n");
	printf("kill - Lists all active sockets then prompts for which one to close.\n");
	printf("list - List all active sockets\n");
}//END FUNCTION


//FUNCTIONS
int main(void) {
	int x = 0;
	char cmd[1024];
	sprintf(ipAddress, "%d.%d.%d.%d", IP_OCTET_1, IP_OCTET_2, IP_OCTET_3, IP_OCTET_4);
	//strcpy(ipAddress, "127.0.0.1");//<------ENTER YOUR INTERNAL IP HERE
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, consoleCommand, NULL);
	serverStart();
	return 0;
}

void *consoleCommand() {
	int activeCount;
	long x;
	char cmd[100];
	void ** holder;
	while (TRUE) {
		
		printf("Console is ready for the next command.\n");
		memset(&cmd, '\0', sizeof(cmd)-1);
		read(STDIN_FILENO, cmd, sizeof(cmd));
		
		if (strncmp(cmd, "list", 4) == 0) {
			//Command: list
			listActiveSockets();
			//End list
		} else if (strncmp(cmd, "kill", 4) == 0) {
			//Command: kill
			activeCount = listActiveSockets();
			if (activeCount > 0) {
				printf("\nWhich socket would you like to kill? (-1 to abort kill)\n");
				memset(&cmd, '\0', sizeof(cmd)-1);
				read(STDIN_FILENO, cmd, sizeof(cmd));
				x =	strtol(cmd, NULL, 10);
				if (x < -1) {
					printf("bad number, aborting!\n");
				} else if (x == -1) {
					printf("-1 entered. Aborting\n");
				} else {
					holder = createISIHolder(getSocket(*socketArray(x)), "close", 0);
					doFunction("alterStruct", holder);
					destroyHolder(holder, 3);
					//alterStruct(getSocket(*socketArray(x)), "close");
				}//END IF
			} else {
				printf("\nNo active sockets to kill\n");
			}//END IF
			//End kill
		} else if (strncmp(cmd, "reload", 6) == 0) {
			//do nothing (for the moment)
		} else if (strncmp(cmd, "monitors", 8) == 0) {
			listNodes(monitorList(0, 1, TRUE));
		} else if (strncmp(cmd, "exit", 4) == 0) {
			//Command: exit
			printf("Shutting down server... \n");
			exit(0);
			//End exit
		} else {
			//Command: (command unknow situation)
			printf("Invalid command.\n");
			printCommandList();
			//End (command unknow situation)
		}//END IF
		printf("\n");
		
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
	void ** holder;
	
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr =	inet_addr(ipAddress);
	serverInfo.sin_port =	htons(CONNECTION_PORT);
	
	s =	socket(AF_INET, SOCK_STREAM, 0);
	rc =	bind(s, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
	listen(s, 10);
	printf("Server Started.\n");
	
	clientInfoLen =	sizeof(clientInfo);
	while (TRUE) {
		clientSocket =	accept(s, &clientInfo, &clientInfoLen);
		holder = createISIHolder(clientSocket, "init", 0);
		i = (int)doFunction("alterStruct", holder);
		destroyHolder(holder, 3);
		//printf("setSocket gave socket #%d a value of %d\n", clientSocket, getSocket(*socketArray(i)));
		printf("Client %d connected\n", clientSocket);
		pthread_create(&((socketArray(i))->t), NULL, clientThread, socketArray(i));
	}//END WHILE LOOP
	
	//pthread_join(thread1, NULL);
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
	
	void ** holder;//holder for arguments using createHolder functions
	
	printf("New thread created\n");
	bytes =	read(getSocket(cli), readBuffer, sizeof(readBuffer)-1);
	printf("Connection received. Parsing headers.\n");
	
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
			holder = createISIHolder(getSocket(cli), "close", 0);
			if ((int)doFunction("alterStruct", holder) == -1) {
			//if ((int)alterStruct(getSocket(cli), "close") == -1) {
				printf("\n\n\t\t **** ERROR: CAUGHT CLOSE ATTEMPT OF BAD SOCKET ****\n");
				printf("\t\t **** IF KILL COMMAND GIVEN FROM CONSOLE IGNORE THIS ****\n\n");
			} else {
				printf("Client #%d Closed.\n", getSocket(cli));
			}//END IF
			destroyHolder(holder, 3);
			break;
		} else if (bytes > 0) {
			/*Byte Check*
			for (j=0; j < bytes; j++) {
				printf("0x%08x\n", readBuffer[j]);
			}//END FOR LOOP
			/**/
			
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
			
			//send the decoded message to the performAction function
			holder = createSCSHolder(writeBuffer, &cli);
			doFunction("performAction", holder);//this is where the magic happens
			destroyHolder(holder, 2);
			
			j=0;
			while (monitorList(j, 1, TRUE) != NULL) {
				holder =	createISIHolder(getSocket(monitorList(j, 1, TRUE)->client), writeBuffer, strlen(writeBuffer));
				doFunction("sendMessage", holder);
				destroyHolder(holder, 3);
				j++;
			}//END WHILE LOOP
			
			//reset buffer to NULL bytes
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
		}//END IF
	}//END WHILE LOOP
	
	printf("Thread ended.\n");
	
	return NULL;
}//END FUNCTION

