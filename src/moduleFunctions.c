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
//this file will be used to make libfunctions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include <dirent.h>
#include <limits.h> //needed for PATH_MAX constant

#include "../include/constants.h"
#include "../include/structs.h"
#include "../include/functions.h"
//#include "../include/globalVars.h"

#ifndef MODULEFUNCTIONS__
#define MODULEFUNCTIONS__

void activateModule(char * name) {
	char activePath[PATH_MAX];
	char availPath[PATH_MAX];
	char buffer[100];
	int buffRead = sizeof(buffer);
	
	getcwd(activePath, sizeof(activePath));
	strcat(activePath, "/modules/active/");
	strcat(activePath, name);
	//strcat(activePath, ".conf");
	
	getcwd(availPath, sizeof(availPath));
	strcat(availPath, "/modules/available/");
	strcat(availPath, name);
	//strcat(availPath, ".conf");
	
	int fd_source, fd_dest;
	if (moduleIsActive(name) == TRUE) {
		printf("Module %s is already active.\n", name);
		return;
	} else {
		printf("Loading module: %s\n", name);
	}//END IF
	
	fd_source = open(availPath, O_RDONLY);
	fd_dest = open(activePath, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	
	buffRead = read(fd_source, buffer, sizeof(buffer));
	while (buffRead > 0) {
		write(fd_dest, buffer, sizeof(buffer));
		memset(&buffer, '\0', sizeof(buffer));
		buffRead = read(fd_source, buffer, sizeof(buffer));
	}//END WHILE LOOP
	
	memset(&buffer, '\0', sizeof(buffer));
	close(fd_dest);
	close(fd_source);
	
	loadModule(name);
	
}//END VOID

void deactivateModule(char * name) {
	int i;
	
	char activePath[PATH_MAX];
	getcwd(activePath, sizeof(activePath));
	strcat(activePath, "/modules/active/");
	strcat(activePath, name);
	//strcat(activePath, ".conf");
	
	if (moduleIsActive(name) == TRUE) {
		printf("Deactivating module: %s\n", name);
		unlink(activePath);
		for (i = 0; i < NUM_OF_SERVERS; i++) {
			if (strcasecmp(servers[i].name, name) == 0) {
				servers[i].isRunning = 0;
				//pthread_cancel(servers[i].t);
				break;
			}//END IF
		}//END FOR LOOP
	}//END IF
	
}//END VOID

int moduleIsActive(char * name) {
	char modPath[PATH_MAX];
	
	getcwd(modPath, sizeof(modPath));
	strcat(modPath, "/modules/active/");
	strcat(modPath, name);
	//strcat(modPath, ".conf");
	
	return ( access( modPath, F_OK ) != -1 ? TRUE : FALSE );
}//END INT

void loadModule(char * name) {
	int i;
	
	if (moduleIsActive(name) == FALSE) {
		printf("cannot load an inactive module.\n");
		return;
	}//END IF
	
	for (i = 0; i < NUM_OF_SERVERS; i++) {
		if (servers[i].name == NULL || strcasecmp(servers[i].name, name) == 0) {
			break;
		}
	}//END FOR LOOP
	
	servers[i].isRunning = 1;
	if (servers[i].name == NULL) {
		servers[i].name = malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(servers[i].name, name);
		pthread_create(&(servers[i].t), NULL, server, NULL);
	}//END IF
	
}//END VOID

char * getModuleList(int isActiveFolder) {
	DIR *dir;
	struct dirent *ent;
	char * moduleList = NULL;
	int modStringLength = 0;
	int i = 0;
	
	char * modPath = NULL;
	
	if (isActiveFolder == TRUE) {
		modPath = malloc(sizeof(char) * (strlen("./modules/active/") + 1));
		strcpy(modPath, "./modules/active/");
	} else {
		modPath = malloc(sizeof(char) * (strlen("./modules/available/") + 1));
		strcpy(modPath, "./modules/available/");
	}//END IF
	//first we want to open the active module directory
	if ((dir = opendir(modPath)) != NULL) {
		
		//as long as we have directories and files to read 
		while ((ent = readdir(dir)) != NULL) {
			
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				modStringLength += strlen(ent->d_name) + 1;
				i++;
			}
			
		}//END WHILE LOOP
		closedir(dir);
	} else {
		printf("Could not open module folder.\n");
	}//END IF
	
	moduleList = malloc(sizeof(char) * (modStringLength + 1));
	strcpy(moduleList, "");
	
	//now we know how long our string needs to be we can loop back over the directory
	if ((dir = opendir(modPath)) != NULL) {
		
		//as long as we have directories and files to read 
		while ((ent = readdir(dir)) != NULL) {
			
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				strcat(moduleList, ent->d_name);
				strcat(moduleList, "\t");
			}
			
		}//END WHILE LOOP
		closedir(dir);
	} else {
		printf("Could not open module folder.\n");
	}//END IF
	
	free(modPath);
	modPath = NULL;
	
	return moduleList;
}

#endif
