/*
 *
 *   Copyright (C) 2015  Michael Cummins
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
 * @date: 12/26/2014 5:15PM EST
 * @compiler:	GCC
 * @32-bit: YES
 * @description:	This program implements a C version of websockets.
 *                When it starts it uses the internal IP of host and specified port
 *                to server websocket requests from client computers.
 *
 *	Websocket info
 * @supportedVersions:	13 (more later?)
 * @numberOfClients:		Dynamic (Linked-List used to manage clients)
 *	@clientsInside:		socketArray function (found in libfunctions.c)
 */

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>

#include "../include/sha1.h"
#include "../include/base64.h"
#include "../include/constants.h"
#include "../include/structs.h"
#include "../include/functions.h"

//GLOBALS
pthread_t * serverThread;
serverStruct servers[10];
pthread_mutex_t consoleLock;
pthread_cond_t condLock;
int isRunning;

//PROTOTYPES
void * server();
void * client(void * s);
void * console();
char * getIPList(int retIndex);

void closeServers();
void closeConsoles();
void closeClients();
void closeMonitors();
void shutdownInit();

int main(int argc, char ** argv) {
	int i;
	for (i=0; i < 10; i++) {
		servers[i].name = NULL;
		servers[i].port = 0;
	}//END FOR LOOP
	
	isRunning = TRUE;
	pthread_mutex_init(&consoleLock, NULL);
	pthread_cond_init(&condLock, NULL);
	serverThread = malloc(sizeof(pthread_t));
	
	//allow us console control without locking up the server.
	/**
	pthread_mutex_lock(&consoleLock);
	pthread_create(&(*serverThread), NULL, console, NULL);
	pthread_cond_wait(&condLock, &consoleLock);
	pthread_mutex_unlock(&consoleLock);
	/**/
	
	pthread_mutex_lock(&consoleLock);
	pthread_create(&(*serverThread), NULL, server, NULL);
	pthread_cond_wait(&condLock, &consoleLock);
	pthread_mutex_unlock(&consoleLock);
	console();
	
	pthread_mutex_destroy(&consoleLock);
	pthread_cond_destroy(&condLock);
	
	//declare functions to run to clean up server (ensure clean server close)
	//these are at the end because they would not run if a client tries to connect
	atexit(closeServers);
	atexit(closeClients);
	atexit(closeMonitors);
	atexit(shutdownInit);
	
	return 0;
}//END main

void * server() {
	pthread_mutex_lock(&consoleLock);
	int rc,
	    serverSocket;
	struct sockaddr_in serverInfo;
	struct sockaddr_in clientInfo;
	int clientSocket,
	    clientInfoLength;
	
	void ** holder;
	
	int optionNumber = 0;
	char ip[16];
	int port;
	
	//set server ip to use
	printf("Please select the option number that has the IP address you want to use for this server.\n\n");
	getIPList(0);
	scanf("%d", &optionNumber);
	strcpy(ip, getIPList(optionNumber));
	
	printf("Enter a port you want the server to use.\n");
	scanf("%d", &port);
	
	
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family =      AF_INET;
	//serverInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
	//serverInfo.sin_port =        htons(CONNECTION_PORT);//to be changed to user entered port / CONNECTION_PORT + active servers
	serverInfo.sin_addr.s_addr = inet_addr(ip);
	serverInfo.sin_port =        htons(port);//to be changed to user entered port / CONNECTION_PORT + active servers
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int));
	rc = bind(serverSocket, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
	listen(serverSocket, 10);
	
	printf("Server Started.\n");
	printf("Host: %s\nPort: %d\n\n", ip, port);
	
	pthread_cond_signal(&condLock);
	pthread_mutex_unlock(&consoleLock);
	while (isRunning) {
		clientSocket = accept(serverSocket, &clientInfo, &clientInfoLength);
		
		holder = createISIHolder(clientSocket, "init", 0);
		doFunction("alterStruct", holder);
		destroyHolder(holder, 3);
		
		printf("Client %d connected\n", clientSocket);
		tellMonitors(clientSocket, "connect", strlen("connect"));
		
		pthread_create(getThread(*getClient(socketArray(clientSocket, 1, FALSE))), NULL, client, getClient(socketArray(clientSocket, 1, FALSE)));
		pthread_detach(*getThread(*getClient(socketArray(clientSocket, 1, FALSE))));
	}//END WHILE LOOP
	
	return NULL;
}

/*
 * TO-DO:
 * 1) Layout function using messages only (done)
 * 2) Create efficient method to read in html headers (in progress)
 * 3) Implement the SHA1 and base64 for the websocket key
 * 4) Write headers to client correctly
 * 5) Open the continuous loop of client reads
 * 6) Implement checks on when to close client socket
 * 7) Do unmasking of bits
 * 8) Do message processing
 * 9) Clear message buffer to prevent left over data in variables
*/
//to make this as efficient as possible, every variable that is not to be used after the handshake needs to be malloced
void * client(void * s) {
	//need to type-cast (void *) to (clientStruct *)
	clientStruct client = *(clientStruct *)s;
	
	int * validHeaders;
	char readBuffer[1024];
	
	printf("Client connection accepted. Retreiving headers.\n");
	
	*validHeaders = processHeaders(getSocket(client));
	
	printf("Headers processed successfully.\n");
	
	printf("Sending response headers to client.\n");
	
	printf("Response headers sent successfully.\n");
	
	printf("Handshake complete. Begin accepting information from client.\n\n");
	//pthread_detach(pthread_self());
	
	printf("Connection to client has been closed.\n");
	
	pthread_exit(NULL);
	//pthread_cancel(pthread_self());
	return NULL;
}

int processHeaders(int socket) {
	int buffSize = 200;
	char readBuffer[buffSize];
	char * finalBuffer = NULL;
	char * holder = NULL;
	int i, bytes;
	
	char method[10];
	     
	
	do {
		bytes = read(socket, readBuffer, sizeof(readBuffer)-1);
		
		if (finalBuffer == NULL) {
			finalBuffer = malloc(sizeof(char) * (bytes + 1));
			strcpy(finalBuffer, readBuffer);
		} else {
			holder = finalBuffer;
			finalBuffer = malloc(sizeof(char) * (strlen(holder) + bytes + 1));
			strcpy(finalBuffer, holder);
			strcat(finalBuffer, readBuffer);
			memset(&holder, '\0', sizeof(holder)-1);
			free(holder);
			holder = NULL;
		}//END IF
		
		memset(&readBuffer, '\0', sizeof(readBuffer));
		
	} while (bytes == buffSize-1);
	
	printf("All Headers\n-------------------\n%s\n------------------------\n", finalBuffer);
	
	free(finalBuffer);
	finalBuffer = NULL;
	
	return 1;
	
	printf("Printing header information:\n\n");
	for (i = 0; i < bytes; i++) {
		printf("%c", readBuffer[i]);
	}//END FOR LOOP
	printf("\n\n");
	return 1;
}//END INT

//OLD client function (will be rewritten)
/**
void * client (void *s) {
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
	
	char	readBuffer[1024],
			secondaryBuffer[1024],
			writeBuffer[1024],
			masks[4];//the buffers
	int bytes, secondaryBytes = 0;//total received bytes.
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
		memset(&readBuffer, '\0', sizeof(readBuffer));
		if (secondaryBytes == 0) {
			bytes =	read(getSocket(cli), readBuffer, sizeof(readBuffer));
		} else {
			memcpy(readBuffer, secondaryBuffer, secondaryBytes);
			bytes = secondaryBytes;
			secondaryBytes = 0;
		}
		
		//printf("bytes read: %d\n", bytes);
		//for (i = 0; i < bytes; i++) {
			//printf("byte #%02d: 0x%08x\n", i, readBuffer[i]);
		//}//END FOR LOOP
		
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
				//printf("0x%08x\n", readBuffer[j]);
				if (j == 0)
					continue;
				if (readBuffer[j] == '\x81' && readBuffer[j-1] == '\x00' && readBuffer[j-2] == '\x00') {
					printf("only using %d of the %d bytes availible for this message\n", j, bytes);
					secondaryBytes = bytes - j;
					//printf("Potential second message attached to this message\nCopying it to the secondary buffer.\n");
					memcpy(secondaryBuffer, readBuffer + j, secondaryBytes);
					break;
				}//END IF
			}//END FOR LOOP
			/**
			
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
			
			tellMonitors(getSocket(cli), writeBuffer, strlen(writeBuffer));
			
			//reset buffer to NULL bytes
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
		}//END IF
	}//END WHILE LOOP
	
	printf("Thread ended.\n");
	
	return NULL;
}//END FUNCTION
/**/

/**
void * console() {
	int activeCount;
	long x;
	char cmd[101];
	void ** holder;
	
	printf("Console activated.\n");
	while (TRUE) {
		memset(&cmd, '\0', sizeof(cmd)-1);
		printf("Console is ready for the next command.\n");
		read(STDIN_FILENO, cmd, sizeof(cmd));
		
		printf("Invalid command.\n");
		//printCommandList();
		
		printf("\n");
	}//END WHILE LOOP
	return NULL;
}
/**/

void * console() {
	int activeCount;
	long x;
	char cmd[101];
	void ** holder;
	
	/**
	//wait for the server to be initialized
	pthread_mutex_lock(&consoleLock);
	pthread_cond_signal(&condLock);
	pthread_cond_wait(&condLock, &consoleLock);
	pthread_mutex_unlock(&consoleLock);
	/**/
	
	printf("Console activated.\n");
	
	while (TRUE) {
		
		memset(&cmd, '\0', sizeof(cmd)-1);
		//pthread_mutex_lock(&consoleLock);
		printf("Console is ready for the next command.\n");
		read(STDIN_FILENO, cmd, sizeof(cmd));
		//pthread_mutex_unlock(&consoleLock);
		
		if (strncmp(cmd, "new server", 10) == 0) {
			
			
			
		} else if (strncmp(cmd, "list", 4) == 0) {
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
					holder = createISIHolder(getSocket(*socketArray(x, 1, TRUE)), "close", 0);
					doFunction("alterStruct", holder);
					destroyHolder(holder, 3);
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
			break;
			//End exit
		} else {
			//Command: (command unknow situation)
			printf("Invalid command.\n");
			getCommandList();
			//End (command unknow situation)
		}//END IF
		printf("\n");
		
		//printf("Number of active servers: %d\n", (servers[0].name == NULL ? 0 : 1));
	}//END WHILE LOOP
	return NULL;
}//END FUNCTION

void getCommandList() {
	printf("\n");
	printf("\t**** Console Command list ****\n");
	printf("exit - End this program.\n");
	printf("help - Displays this command list\n");
	printf("kill - Lists all active sockets then prompts for which one to close.\n");
	printf("list - List all active sockets\n");
}//END FUNCTION

int listActiveSockets() {
	int i;
	clientStruct client;
	i=0;
	clientNode * node =	NULL;
	
	node =	socketArray(0, 1, TRUE);
	printf("\t* List of active sockets *\n\n");
	while (node != NULL) {
		client = *getClient(node);
		if (getActive(client) == TRUE) {
			printf("%d: Socket #%03d (%s)%c", i+1, getSocket(client), getName(client), (i % 2 == 0 ? '\t' : '\n'));
			i++;
		}//END IF
		node =	node->next;
	}//END WHILE LOOP
	if (i == 0)
		printf("(no active sockets)");
	if (i % 2 == 0)
		printf("\n");
	
	printf("\n\t* End of list *\n");
	return i;
}//END FUNCTION

char * getIPList(int retIndex) {
	int i;
	struct ifaddrs * ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];
	
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(1);
	}//END IF
	
	if (retIndex == 0)
		printf("**** IP SELECTION LIST***\n");
	
	i = 1;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}//END IF
		
		family = ifa->ifa_addr->sa_family;
		
		if (__ISWINDOWS__ == 0 && retIndex == 0) {
		printf("%-8s %s (%d)\n",
			ifa->ifa_name,
			(family == AF_INET) ? "AF_INET" :
			(family == AF_INET6) ? "AF_INET6" : "???",
			family);
		}
		if (family == AF_INET) {// || family == AF_INET6) {
			
			s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6)),
				host, NI_MAXHOST,
				NULL, 0, NI_NUMERICHOST);
			
			if (s != 0) {
				printf("getnameinfo() failed.\n");
				exit(1);
			}//END IF
			
			if (retIndex > 0) {
				if (i == retIndex) {
					freeifaddrs(ifaddr);
					return host;
				}//END IF
			} else {
				printf("Option %d:\tAddress: <%s>\n", i, host);
			}//END IF
			
			//printf("Interface: <%-8s>\n", ifa->ifa_name);
			//printf("\tAddress: <%s>\n", host);
			//printf("\n");
			i++;
		}//END IF
		
	}//END FOR LOOP
	
	freeifaddrs(ifaddr);
	return NULL;
}//END CHAR

//BELOW ARE FUNCTIONS TO BE RUN WHEN PROGRAM ENDS

void closeServers() {
	int i;
	printf("Closing all servers\n");
	for (i = 0; i < 10; i++) {
		if (servers[i].name == NULL) {
			continue;
		}//END IF
		printf("Closing server: %s\n", servers[i].name);
		memset(&servers[i].name, '\0', sizeof(servers[i].name)-1);
		free(servers[i].name);
		servers[i].name = NULL;
	}//END FOR LOOP
	
	printf("Closing main server.\n");
	pthread_cancel(*serverThread);
}//END closeServers

void closeClients() {
	clientNode * head = NULL;
	for (head = socketArray(0, 1, TRUE); head != NULL; head = head->next) {
		pthread_cancel(*getThread(*getClient(head)));
	}//END FOR LOOP
}//END closeClients

void closeMonitors() {
	
}//END closeMonitors

void shutdownInit() {
	printf("exiting program...\n\n");
}
