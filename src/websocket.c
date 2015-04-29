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

#include <dirent.h>

#include <limits.h> //needed for PATH_MAX constant

#include "../include/sha1.h"
#include "../include/base64.h"
#include "../include/constants.h"
#include "../include/structs.h"
#include "../include/functions.h"

//GLOBALS
pthread_t * serverThread;
pthread_t * outputThread;
pthread_mutex_t consoleLock;
pthread_cond_t condLock;
serverStruct servers[10];
int isRunning;

//PROTOTYPES
void * server();
void * client(void * s);
void * console();
char * getIPList(int retIndex);

void closeServers();
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
	outputThread = malloc(sizeof(pthread_t));
	
	/*thread for switching printf around*/
	startServers();
	pthread_create(&(*outputThread), NULL, printRedirect, NULL);
	
	//allow us console control without locking up the server.
	/**
	pthread_mutex_lock(&consoleLock);
	pthread_create(&(*serverThread), NULL, server, NULL);
	pthread_cond_wait(&condLock, &consoleLock);
	pthread_mutex_unlock(&consoleLock);
	console();
	
	pthread_mutex_destroy(&consoleLock);
	pthread_cond_destroy(&condLock);
	/**/
	console();
	
	//declare functions to run to clean up server (ensure clean server close)
	//these are at the end because they would not run if a client tries to connect
	atexit(closeServers);
	atexit(closeClients);
	atexit(closeMonitors);
	atexit(shutdownInit);
	
	return 0;
}//END main

void * printRedirect() {
	struct sockaddr_in connInfo;
	struct sockaddr_in clientInfo;
	int isRunning = 1;
	int in_holder = 0;
	int out_holder = 1;
	//int port =	4332;
	int port =	80;
	
	int connSocket,
	    clientSocket,
	    clientInfoLength;
	
	char readBuffer[512];
	char writeBuffer[512];
	char * str = NULL;
	int readBytes, writeBytes;
	
	pid_t cpid;
	
	memset(&connInfo, 0, sizeof(connInfo));
	connInfo.sin_family =      AF_INET;
	connInfo.sin_addr.s_addr = inet_addr("192.168.1.113");
	//connInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
	//connInfo.sin_port =        htons(CONNECTION_PORT);//to be changed to user entered port / CONNECTION_PORT + active servers
	connInfo.sin_port =        htons(port);//to be changed to user entered port / CONNECTION_PORT + active servers
	
	connSocket = socket(AF_INET, SOCK_STREAM, 0);
	bind(connSocket, (struct sockaddr *) &connInfo, sizeof(connInfo));
	listen(connSocket, 1);
	
	in_holder = dup(0);
	out_holder = dup(1);
	while (isRunning) {
		clientSocket =	accept(connSocket, &clientInfo, &clientInfoLength);
		readBytes = read(clientSocket, readBuffer, sizeof(readBuffer));
		if (strcmp(readBuffer, "Validate Client: COP-test-run") != 0) {
			strcpy(writeBuffer, "Invalid connection string. Terminating connection.");
			if (write(clientSocket, writeBuffer, strlen(writeBuffer)) <= 0) {
				printf("Failed to write. Closing connection anyways.\n");
			}//END IF
			close(clientSocket);
			clientSocket = -1;
		}//END IF
		
		//printf("test\n");
		memset(readBuffer, '\0', strlen(readBuffer));
		
		if (clientSocket != -1) {
			printf("Switching from Console mode to Admin mode.\n");
			dup2(clientSocket, 0);
			dup2(clientSocket, 1);
			while (TRUE) {
				readBytes = read(STDIN_FILENO, readBuffer, sizeof(readBuffer));
				if (readBytes <= 0) {
					break;
				}//END IF
				
				if (strcasecmp(readBuffer, "getAvailableModules") == 0) {
					//printf("we are sending available modules.\n");
					str = getModuleList(0);
					printf("%s\n", str);
					free(str);
					str = NULL;
				} else if (strcasecmp(readBuffer, "getActiveModules") == 0) {
					//printf("we are sending active modules.\n");
					str = getModuleList(1);
					printf("%s\n", str);
					free(str);
					str = NULL;
				} else if (strncasecmp(readBuffer, "activateModule", 14) == 0) {
					dup2(out_holder, 1);
					activateModule(readBuffer + 15);
					dup2(clientSocket, 1);
				} else if (strncasecmp(readBuffer, "deactivateModule", 16) == 0) {
					dup2(out_holder, 1);
					deactivateModule(readBuffer + 17);
					dup2(clientSocket, 1);
				}//END IF
				
				dup2(out_holder, 1);
				printf("string: '%s'\n", readBuffer);
				printf("readBytes = %d\n", readBytes);
				dup2(clientSocket, 1);
				
				memset(readBuffer, '\0', strlen(readBuffer));
			}//END WHILE LOOP
			dup2(in_holder, 0);
			dup2(out_holder, 1);
			close(clientSocket);
			printf("Switching from Admin mode to Console mode.\n");
		}//END IF
		
		memset(readBuffer, '\0', sizeof(readBuffer));
		
	}//END WHILE LOOP
	
	close(in_holder);
	close(out_holder);
	
	return NULL;
}//END VOID

void * server() {
	//pthread_mutex_lock(&consoleLock);
	int rc,
	    serverSocket;
	struct sockaddr_in serverInfo;
	struct sockaddr_in clientInfo;
	int clientSocket,
	    clientInfoLength;
	
	void ** holder;
	
	int optionNumber = -1;
	char ip[16];
	int port;
	
	char * path = NULL;
	char readBuffer[100];
	FILE * fi = NULL;
	
	//using port in place of i to save memory
	for (port = 0; port < NUM_OF_SERVERS; port++) {
		if (pthread_equal(servers[port].t, pthread_self()) != 0) {
			optionNumber = port;
			break;
		}//END IF
	}//END FOR LOOP
	
	if (optionNumber == -1)
		return NULL;
	
	path = malloc(sizeof(char) * (strlen("./modules/active/") + strlen(servers[optionNumber].name) + 1));
	strcpy(path, "./modules/active/");
	strcat(path, servers[optionNumber].name);
	fi = fopen(path, "r");
	
	while (feof(fi) == 0) {
		fscanf(fi, "%s", readBuffer);
		
		if (strcasecmp(readBuffer, "address") == 0) {
			fscanf(fi, "%s", ip);
			strcpy(servers[optionNumber].ip, ip);
		} else if (strcasecmp(readBuffer, "port") == 0) {
			fscanf(fi, "%d", &port);
			servers[optionNumber].port = port;
		} else if (strcasecmp(readBuffer, "path") == 0) {
			memset(readBuffer, '\0', strlen(readBuffer));
			fscanf(fi, "%s", readBuffer);
			servers[optionNumber].path = malloc(sizeof(char) * (strlen(readBuffer) + 1));
			strcpy(servers[optionNumber].path, readBuffer);
		} else {
			memset(readBuffer, '\0', strlen(readBuffer));
			fscanf(fi, "%s", readBuffer);
		}//END IF
		memset(readBuffer, '\0', strlen(readBuffer));
		
	}//END WHILE LOOP
	
	fclose(fi);
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin_family =      AF_INET;
	serverInfo.sin_addr.s_addr = inet_addr(servers[optionNumber].ip);
	serverInfo.sin_port =        htons(servers[optionNumber].port);//to be changed to user entered port / CONNECTION_PORT + active servers
	
	/*NO MODULE METHOD*
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
	
	/**/
	
	
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	//setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int));
	rc = bind(serverSocket, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
	listen(serverSocket, 10);
	
	printf("Server Started.\n");
	printf("Host: %s\nPort: %d\n\n", servers[optionNumber].ip, servers[optionNumber].port);
	
	//pthread_cond_signal(&condLock);
	//pthread_mutex_unlock(&consoleLock);
	while (servers[optionNumber].isRunning) {
		clientSocket = accept(serverSocket, &clientInfo, &clientInfoLength);
		
		holder = createISIHolder(clientSocket, "init", 0);
		doFunction("alterStruct", holder);
		destroyHolder(holder, 3);
		
		printf("Client %d connected\n", clientSocket);
		tellMonitors(clientSocket, "connect", strlen("connect"));
		
		rc = pthread_create(&(getClient(socketArray(clientSocket, 1, FALSE))->t), NULL, client, getClient(socketArray(clientSocket, 1, FALSE)));
		if (rc) {
			printf("\n\tERROR: return code from pthread_create is %d \n", rc);
			exit(1);
		}//END IF
	}//END WHILE LOOP
	
	//release resources
	memset(servers[optionNumber].name, '\0', strlen(servers[optionNumber].name));
	free(servers[optionNumber].name);
	servers[optionNumber].name = NULL;
	
	memset(servers[optionNumber].path, '\0', strlen(servers[optionNumber].path));
	free(servers[optionNumber].path);
	servers[optionNumber].path = NULL;
	
	close(serverSocket);
	
	return NULL;
}

/*
 * TO-DO:
 * 1) Layout function using messages only (done)
 * 2) Create efficient method to read in html headers (done)
 * 3) Implement the SHA1 and base64 for the websocket key (done)
 * 4) Write headers to client correctly (done)
 * 5) Open the continuous loop of client reads (in progress)
 * 6) Implement checks on when to close client socket
 * 7) Do unmasking of bits
 * 8) Do message processing
 * 9) Clear message buffer to prevent left over data in variables
*/
//to make this as efficient as possible, every variable that is not to be used after the handshake needs to be malloced
void * client(void * s) {
	//need to type-cast (void *) to (clientStruct *)
	clientStruct client = *(clientStruct *)s;
	pthread_detach(pthread_self());
	
	int validHeaders;
	int i;
	
	int isConnected = 1;
	char * writeBuffer = NULL;
	char readBuffer[1024];
	char extraBuffer[1024];
	int readBytes,
	    writeBytes,
	    extraBytes = 0;
	int maskIndex;
	char masks[4];
	
	void ** holder;
	
	printf("Client connection accepted. Retreiving headers.\n");
	
	validHeaders = processHeaders(getSocket(client));
	
	if (validHeaders == 0) {
		holder = createISIHolder(getSocket(client), "close", 0);
		(int)doFunction("alterStruct", holder);
		destroyHolder(holder, 3);
		pthread_exit(NULL);
		return NULL;
	}//END IF
	
	printf("Response headers sent successfully.\n");
	
	printf("Handshake complete. Begin accepting information from client.\n\n");
	
	while (isConnected) {
		memset(&readBuffer, '\0', sizeof(readBuffer));
		
		if (extraBytes == 0) {
			readBytes = read(getSocket(client), readBuffer, sizeof(readBuffer));
		} else {
			memcpy(readBuffer, extraBuffer, extraBytes);
			readBytes = extraBytes;
			extraBytes = 0;
		}//END IF
		
		//printf("bytes read: %d\n", readBytes);
		//for (i = 0; i < readBytes; i++) {
			//printf("byte #%02d: 0x%08x\n", i, readBuffer[i]);
		//}//END FOR LOOP
		
		if (readBytes <= 0 || readBuffer[0] == '\x88') {
			holder = createISIHolder(getSocket(client), "close", 0);
			if ((int)doFunction("alterStruct", holder) == -1) {
				printf("\n\n\t\t **** ERROR: CAUGHT CLOSE ATTEMPT OF BAD SOCKET ****\n");
				printf("\t\t **** IF KILL COMMAND GIVEN FROM CONSOLE IGNORE THIS ****\n\n");
			} else {
				printf("Client #%d Closed.\n", getSocket(client));
			}//END IF
			destroyHolder(holder, 3);
			isConnected = 0;
			break;
		} else if (readBytes > 0) {
			
			maskIndex = 2;
			writeBytes = (readBuffer[1] & 0x7f);
			//printf("Value of writeBytes: %d\n", writeBytes);
			if ((readBuffer[1] & 0x7f) == 126) {
				maskIndex = 4;
				writeBytes = ( (readBuffer[2] * 256) + readBuffer[3] );
			} else if ((readBuffer[1] & 0x7f) == 127) {
				maskIndex = 10;
			}//END IF
			
			masks[0] = readBuffer[maskIndex++];
			masks[1] = readBuffer[maskIndex++];
			masks[2] = readBuffer[maskIndex++];
			masks[3] = readBuffer[maskIndex++];
			
			writeBuffer = malloc(sizeof(char) * (writeBytes + 1));
			memset(writeBuffer, '\0', writeBytes + 1);
			for(i=maskIndex; i < readBytes; i++) {
				writeBuffer[i - maskIndex] = readBuffer[i] ^ masks[(i - maskIndex) % 4];
				//printf("current character: %c\n", writeBuffer[i - maskIndex]);
			}//END FOR LOOP
			printf("Message from socket #%d: \"%s\"\n", getSocket(client), writeBuffer);
			
			//send the decoded message to the performAction function
			holder = createSCSHolder(writeBuffer, &client);
			doFunction("performAction", holder);//this is where the magic happens
			destroyHolder(holder, 2);
			
			//tellMonitors(getSocket(client), writeBuffer, strlen(writeBuffer));
			
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
			free(writeBuffer);
			writeBuffer = NULL;
		}//END IF
	}//END WHILE LOOP
	
	pthread_exit(NULL);
	return NULL;
}

void startServers() {
	DIR *dir;
	struct dirent *ent;
	char * filename = NULL;
	int i = 0;
	
	//first we want to open the active module directory
	if ((dir = opendir("./modules/active/")) != NULL) {
		
		//as long as we have directories and files to read 
		while ((ent = readdir(dir)) != NULL) {
			
			filename = malloc(sizeof(char) * (strlen(ent->d_name) + 1));
			strcpy(filename, ent->d_name);
			if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
				servers[i].name = filename;
				servers[i].isRunning = 1;
				pthread_create(&(servers[i].t), NULL, server, NULL);
				i++;
			} else {
				free(filename);
				filename = NULL;
			}//END IF
			
		}//END WHILE LOOP
		closedir(dir);
	} else {
		printf("Could not open directory.\n");
	}//END IF
	
}//END startServers

int processHeaders(int socket) {
	int buffSize = 200;
	char readBuffer[buffSize];
	char * response = NULL;
	char * finalBuffer = NULL;
	char * holder = NULL;
	int i, bytes;
	
	char method[10];
	int lineIndex = 0;
	int currIndex = 0;
	
	//used for processing headers
	int validHeader = 1;
	char * currHeader = NULL;
	char * currData = NULL;
	
	//used to compute key
	int j;
	SHA1Context sha;
	base64_encodestate b64_ctx;
	char * key = NULL;
	unsigned char midKey[20];
	char endKey[40];
	
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
	
	bytes = strlen(finalBuffer);
	
	//Check if the method is GET
	if (strncmp(finalBuffer, "GET", 3) != 0) {
		response = malloc(sizeof(char) * 46);
		strcpy(response, "HTTP/1.1 405 Method Not Allowed\r\nAllow: GET\r\n");
		write(socket, response, strlen(response));
		memset(&response, '\0', sizeof(response)-1);
		free(response);
		response = NULL;
		return 0;
	}//END IF
	
	//Check if the request has a leading /
	if (finalBuffer[4] != '/') {
		response = malloc(sizeof(char) * 27);
		strcpy(response, "HTTP/1.1 400 Bad Request\r\n");
		write(socket, response, strlen(response));
		memset(&response, '\0', sizeof(response)-1);
		free(response);
		response = NULL;
		return 0;
	}//END IF
	
	//get the length of the path used in the request
	for (i = 5; finalBuffer[i] != ' '; i++) {
		currIndex++;
	}//END FOR LOOP
	
	if (currIndex > 0) {
		for (i = 1; i <= NUM_OF_SERVERS; i++) {
			if (i == NUM_OF_SERVERS) {
				break;
			}//END IF
			if (servers[i].name == NULL) {
				continue;
			}//END IF
			if (strncmp(finalBuffer + 5, servers[i].name, currIndex) == 0) {
				break;
			}//END IF
		}//END FOR LOOP
		
		if (i == NUM_OF_SERVERS) {
			response = malloc(sizeof(char) * 25);
			strcpy(response, "HTTP/1.1 404 Not Found\r\n");
			write(socket, response, strlen(response));
			memset(&response, '\0', sizeof(response)-1);
			free(response);
			response = NULL;
			return 0;
		}//END IF
	}//END IF
	
	//get the start position of the first header
	for (i = 6; i < bytes; i++) {
		if (strncmp(finalBuffer + i, "\r\n", 2) == 0) {
			currIndex = i+2;
			break;
		}//END IF
	}//END FOR LOOP
	
	//run over the headers
	for (i = currIndex; i < bytes; i++) {
		if (strncmp(finalBuffer + i, "\r\n", 2) == 0) {
			if (i == currIndex)
				break;
			currData = malloc(sizeof(char) * (i - currIndex + 1));
			memcpy(currData, finalBuffer + currIndex, (i - currIndex));
			currData[(i-currIndex)] = '\0';
			
			if (strcmp(currHeader, "Upgrade") == 0) {
				if (strcasecmp(currData, "websocket") != 0) {
					validHeader = 0;
				}//END IF
			}
			if (strcmp(currHeader, "Connection") == 0) {
				if (strcasecmp(currData, "Upgrade") != 0) {
					validHeader = 0;
				}//END IF
			}
			if (strcmp(currHeader, "Sec-WebSocket-Key") == 0) {
				key = malloc(sizeof(char) * (strlen(currData) + strlen("258EAFA5-E914-47DA-95CA-C5AB0DC85B11") + 1));
				SHA1Reset(&sha);
				strcpy(key, currData);
				strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
				SHA1Input(&sha, key, strlen(key));
				SHA1Result(&sha);
				for(j = 0; j < 5; ++j) {	// put in correct byte order before memcpy.
					sha.Message_Digest[j] = ntohl(sha.Message_Digest[j]);
				}//END FOR LOOP
				memcpy(midKey, (unsigned char*)sha.Message_Digest, 20);
				
				memset(&key, '\0', strlen(key) - 1);
				free(key);
				key = NULL;
				
				// encode `midKey' in base 64, into `endKey'.
				base64_init_encodestate(&b64_ctx);
				j = base64_encode_block((const char*)midKey, 20, endKey, &b64_ctx);
				base64_encode_blockend(endKey + j, &b64_ctx);
				
			}//END IF
			if (strcmp(currHeader, "Sec-WebSocket-Version") == 0) {
				if (strcmp(currData, "13") != 0) {
					validHeader = 0;
				}//END IF
			}//END IF
			
			//printf("Current Header: %s\nCurrent Data: %s\n\n", currHeader, currData);
			memset(&currData, '\0', sizeof(currData)-1);
			free(currData);
			currData = NULL;
			if (currHeader != NULL) {
				memset(&currHeader, '\0', sizeof(currHeader)-1);
				free(currHeader);
				currHeader = NULL;
			}//END IF
			
			if (validHeader == 0) break;
			currIndex = i+2;
			i++;
			continue;
		}//END IF
		if (strncmp(finalBuffer + i, ": ", 2) == 0) {
			currHeader = malloc(sizeof(char) * (i - currIndex + 1));
			memcpy(currHeader, finalBuffer + currIndex, (i - currIndex));
			currHeader[(i-currIndex)] = '\0';
			currIndex = i+2;
		}//END IF
	}//END FOR LOOP
	
	//this is where the final decision comes in if we deny or reject the connection
	if (validHeader) {
		response = malloc(sizeof(char) * (99 + strlen(endKey) + 1));
		strcpy(response, "HTTP/1.1 101 Switching Protocols\r\n");
		strcat(response, "Upgrade: websocket\r\n");
		strcat(response, "Connection: Upgrade\r\n");
		//write the sec-ws-accept to buffer
		strcat(response, "Sec-WebSocket-Accept: ");
		strcat(response, endKey);
		strcat(response, "\r\n");
		
		write(socket, response, strlen(response));
		memset(&response, '\0', sizeof(response)-1);
		free(response);
		response = NULL;
		return 1;
	} else {
		response = malloc(sizeof(char) * 27);
		strcpy(response, "HTTP/1.1 400 Bad Request\r\n");
		write(socket, response, strlen(response));
		memset(&response, '\0', sizeof(response)-1);
		free(response);
		response = NULL;
		return 0;
	}//END IF
	
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
	char * arg = NULL;
	void ** holder;
	
	/**
	//wait for the server to be initialized
	pthread_mutex_lock(&consoleLock);
	pthread_cond_signal(&condLock);
	pthread_cond_wait(&condLock, &consoleLock);
	pthread_mutex_unlock(&consoleLock);
	/**/
	
	//system("ls modules/available/*.conf | sed 's_.*/__' | sed 's_.conf__' | tail");
	
	printf("Console activated.\n");
	
	while (TRUE) {
		
		memset(&cmd, '\0', sizeof(cmd));
		printf("Console is ready for the next command.\n");
		read(STDIN_FILENO, cmd, sizeof(cmd));
		getWord(&arg, cmd);
		
		if (arg == NULL) {
			//do nothing
		} else if (strcasecmp(arg, "load") == 0) {
			getWord(&arg, cmd);
			if (arg == NULL) {
				//system("ls -1 ./modules/available | sed 's_.conf__' | tr '\\n' ' '");
			} else {
				activateModule(arg);
			}//END IF
		} else if (strcasecmp(arg, "unload") == 0) {
			getWord(&arg, cmd);
			if (arg == NULL) {
				
			} else {
				deactivateModule(arg);
			}//END IF
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
				x = x - 1;
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
		} else if (strcasecmp(arg, "exit") == 0) {
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
		
		getWord(&arg, "");
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
		pthread_cancel(getClient(head)->t);
	}//END FOR LOOP
}//END closeClients

void closeMonitors() {
	
}//END closeMonitors

void shutdownInit() {
	printf("exiting program...\n\n");
}
