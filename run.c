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
//DO NOT COMPILE AS A LIBRARY, FUNCTION, OR APP
//USE gcc -o run run.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void testPrint(int line, const char *func) {
	printf("%s is at line %d\n", func, line);
}

int main(int argc, char ** argv) {
	char * str;//max filename length = 256 + 3 for ./ and + 1 for \0 on end
	
	/**
	int i;
	printf("ARGUMENTS LIST:\n");
	for (i=0; i < argc; i++) {
		printf("arg[%d]:\t\"%s\"\n", i, argv[i]);
	}//END FOR LOOP
	return 0;
	/**/
	
	if (argc == 2) {
		if (strncmp(argv[0], "./", 2) == 0) {
			str = malloc(sizeof(char) * ( strlen("./setupEnv.sh run ") + strlen(argv[1]) + 1 ));
			strcpy(str, "./setupEnv.sh run ");
			strcat(str, argv[1]);
			system(str);
		} else {
			str = malloc(sizeof(char) * ( strlen("cmd /C \"set PATH=%PATH%;%cd%\\lib&&bin\\\"") + strlen(argv[1]) + 1 ));
			strcpy(str, "cmd /C \"set PATH=%PATH%;%cd%\\lib&&bin\\");
			strcat(str, argv[1]);
			strcat(str, "\"");
			system(str);
		}//END IF
		free(str);
		return 0;
	} else if (argc == 1) {
		if (strncmp(argv[0], "./", 2) == 0) {
			str = malloc(sizeof(char) * ( strlen("./setupEnv.sh run websocket") + 1 ));
			strcpy(str, "./setupEnv.sh run websocket");
			system(str);
		} else {
			str = malloc(sizeof(char) * ( strlen("cmd /C \"set PATH=%PATH%;%cd%\\lib&&bin\\websocket\"") + 1 ));
			strcpy(str, "cmd /C \"set PATH=%PATH%;%cd%\\lib&&bin\\websocket\"");
			system(str);
		}//END IF
		free(str);
		return 0;
	}//END IF
	
	
	return 1;
}//END MAIN
