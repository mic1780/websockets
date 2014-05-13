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

//INCLUDES
#include "include/functions.h"

/**
int main(int argc, char ** argv) {
	int i;
	printf("There are %d arguments.\n", argc);
	for (i=0; i < argc; i++) {
		printf("argument #%d: %s\n", i+1, argv[i]);
	}//END FOR LOOP
	return 0;
}//END FUNCTION
/*/

/**/
int main(int argc, char ** argv) {
	//int i;
	//if (argc > 1) {
		//if (strncmp(argv[1], "sendall", 7) == 0) {
			printf("this pid = %d\t\t parent pid = %d\n", getpid(), getppid());
			printf("My clientStruct pos #%d is %s\n", getSocket(temp[0]), (getActive(temp[0]) == 0 ? "inactive" : "active"));
			sendMessage(getSocket(temp[0]), "Hello World!", 12);
		//}//END IF
	//}//END IF
	
	return 0;
}//END FUNCTION
/**/
