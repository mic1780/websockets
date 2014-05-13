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
 * @date: 4/11/2013 12:34PM
 * @compiler:	GCC
 * @32-bit: YES
 * @description:	This program implements a C version of websockets.
 *                When it starts it uses the internal IP of host and specified port
 *                to server websocket requests from client computers.
 *
 *	Websocket info
 * @supportedVersions:	13
 * @numberOfClients:		Dynamic
 *	@clientsInside:		static array
 */

//INCLUDES
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
#include "functions.h"

//GLOBALS
static int serv;
static char ipAddress[16];



//FUNCTIONS
int main(void) {
	int x = 0;
	char cmd[1024];
	strcpy(ipAddress, "127.0.0.1");//<------ENTER YOUR INTERNAL IP HERE
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, consoleCommand, NULL);
	serverStart();
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
	serverInfo.sin_addr.s_addr =	inet_addr(ipAddress);
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
			
			//send the decoded message to the performAction function
			performAction(writeBuffer, &cli);
			execute("./execute.exe sendall Hello World!", cli, NULL, NULL, NULL);
			
			//reset buffer to NULL bytes
			memset(&writeBuffer, '\0', sizeof(writeBuffer));
		}//END IF
	}//END WHILE LOOP
	
	printf("Thread ended.\n");
	
	return NULL;
}//END FUNCTION

