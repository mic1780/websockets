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
#include <dlfcn.h>
#include "../include/structs.h"
#include "../include/constants.h"

#ifndef LIBFUNCS__
#define LIBFUNCS__
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

int getMonitor(clientStruct s) {
	return s.isMonitor;
}//END FUNCTION
void setMonitor(clientStruct * s, int bitFlag) {
	s->isMonitor =	bitFlag;
}//END FUNCTION

void initializeSockets(clientStruct * sock) {
	int i;
	for (i=0; i < NUM_OF_CLIENTS; i++) {
		setSocket(&sock[i], 0);
		setActive(&sock[i], FALSE);
		setMonitor(&sock[i], FALSE);
		sock[i].name = NULL;
	}//END FOR LOOP
}//END FUNCTION

clientStruct * socketArray(int position) {
	static clientStruct temp[NUM_OF_CLIENTS];
	static int initialized = 0;
	
	if (initialized == 0) {
		printf("\n\n\t\t **** INITIALIZING temp ****\n\n");
		initialized++;
		initializeSockets(temp);
	}
	return &temp[position];
}//END FUNCTION

clientNode * findNode(clientNode * head, int sock, int sockIsIndex) {
	int i;
	clientNode * ptr = NULL;
	
	if (head == NULL)
		return NULL;
	
	ptr = head;
	
	//when we want to get a specific index in the linked list, this block is run
	if (sockIsIndex) {
		//if (sock >= size) {
			//return NULL;
		//}//END IF
		for (i=0; i < sock; i++) {
			if (ptr == NULL)
				break;
			ptr =	ptr->next;
		}//END FOR LOOP
		return ptr;
	}//END IF
	
	//when we are looking for a specific socket in the list, this block is run
	while (ptr != NULL) {
		if (getSocket(ptr->client) == sock)
			return ptr;
		ptr =	ptr->next;
	}//END WHILE LOOP
	
	return NULL;
}//END FUNCTION

void createNode(clientNode ** head, int sock, int * size) {
	clientNode * node =	NULL;
	if (*head == NULL) {
		node = NULL;
	} else {
		node = *head;
	}
	
	
	if (node == NULL) {
		node =			malloc(sizeof(clientNode));
		setSocket(&(node->client), sock);
		setActive(&(node->client), TRUE);
		node->next =	NULL;
		node->prev =	NULL;
		*size++;
		*head = node;
	} else {
		createNode(&((*head)->prev->next), sock, size);
		(*head)->prev->next->prev = (*head)->prev;
		(*head)->prev =	node->prev->next;
	}//END IF
	
}//END FUNCTION

void destroyNode(clientNode ** head, int sock, int * size) {
	clientNode * ptr = NULL;
	
	if (head == NULL)
		return head;
	
	ptr = *head;
	while (ptr != NULL) {
		if (getSocket(ptr->client) == sock) {
			if (ptr->prev->next == NULL) {
				//first node is it
				*head = (*head)->next;
				if (*head != NULL) {
					(*head)->prev =	ptr->prev;
				}//END IF
			} else {
				ptr->prev->next = ptr->next;
			}//END IF
			free(ptr);
			*size--;
			return;
		}//END IF
		ptr =	ptr->next;
	}//END WHILE LOOP
	
	if (ptr == NULL) {
		printf("\nWARNING: Could not find socket in node list: %d\n\n", sock);
		return;
	}//END IF
}//END FUNCTION

void listNodes(clientNode * head) {
	if (head == NULL) {
		printf("\nList has no nodes\n\n");
		return;
	}//END IF
	printf("\nList of nodes\n");
	while (head != NULL) {
		printf("Socket:\t%d\n", getSocket(head->client));
		head = head->next;
	}//END WHILE LOOP
	printf("\n");
}//END FUNCTION

clientNode * monitorList(int sock, int method, int sockIsIndex) {
	int i;
	static clientNode * head =	NULL;
	static int size =				0;
	clientNode * ptr =			NULL;
	
	method =	method & 7;//only use first 3 bits;
	
	//find method
	if (method & 1 == 1) {
		ptr =	findNode(head, sock, sockIsIndex);
		if (ptr != NULL)
			return ptr;
	}//END IF
	
	//create method
	if ((method & 2) == 2 && sockIsIndex == FALSE) {
		createNode(&head, sock, &size);
		if (head->prev == NULL)
			head->prev = head;
	}//END IF
	
	//destroy method
	if ((method & 4) == 4 && sockIsIndex == FALSE) {
		destroyNode(&head, sock, &size);
	}//END IF
	
	return NULL;
}//END FUNCTION

//STRUCT HELPER FUNCTIONS
void ** createHolder(int sock, char *s, int len) {
	void ** holder;
	holder = malloc(sizeof(void *) * 3);
	holder[0] = (int *)sock;
	holder[1] = (char *)s;
	holder[2] = (int *)len;
	return holder;
}

void ** createISIHolder(int sock, char *s, int len) {
	void ** holder;
	holder = malloc(sizeof(void *) * 3);
	holder[0] = (int *)sock;
	holder[1] = (char *)s;
	holder[2] = (int *)len;
	return holder;
}

void ** createSCSHolder(char *s, clientStruct *cli) {
	void ** holder;
	holder = malloc(sizeof(void *) * 2);
	holder[0] = (char *)s;
	holder[1] = (clientStruct *)cli;
	return holder;
}

void destroyHolder(void ** holder, int len) {
	int i;
	for (i=0; i < len; i++) {
		holder[i] = NULL;
	}//END FOR LOOP
	free(holder);
	holder = NULL;
}

int checkForError(char *error) {
	if (error != NULL) {
		printf("ERROR: %s\n", error);
		return 1;
		//exit(EXIT_FAILURE);
	}//END IF
	
	return 0;
}//END IF

void * doFunction(char * fName, void ** argv) {
	void *handle;
	void * funcPtr;
	void * returnVal;
	FILE * filePtr = NULL;
	
	typedef void * (*funcType)(char *, void **);
	if (__ISWINDOWS__) {
		handle = dlopen("lib/libcallFunction.dll", RTLD_LAZY);
	} else {
		handle = dlopen("so/libcallFunction.so", RTLD_LAZY);
	}//END IF
	
	if (!handle) {
		checkForError(dlerror());
		return NULL;
	}//END IF
	
	dlerror();
	
	funcPtr = dlsym(handle, "callFunction");
	if (checkForError(dlerror()) == 0) {
		funcType callFunction = (funcType) funcPtr;
		returnVal = (callFunction)(fName, argv);
	}//END IF
	
	dlclose(handle);
	handle = NULL;
	return returnVal;
	
}

void testPrint(int line, char * func) {
	printf("Function %s is on line %d.\n", func, line);
}//END FUNCTION
#endif
