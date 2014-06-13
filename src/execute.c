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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "../include/structs.h"
#include "../include/functions.h"

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
			//execlp("/bin/bash", "./execute.exe", "-c", command, NULL);
			_exit(0);
			
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
				//sendMessage(getSocket(s), buf, strlen(buf));
				memset(&buf, '\0', sizeof(buf));
				free(buf);
				buf =	NULL;
				buf =	malloc(sizeof(char) * 100);
			}//END WHILE LOOP
			memset(&buf, '\0', sizeof(buf));
			free(buf);
			buf = NULL;
			
			if (in)  *in  = fdopen(fd[1], "wb"); else close(fd[1]);
			if (out) *out = fdopen(fd[2], "rb"); else close(fd[2]);//stdout ends here
			if (err) *err = fdopen(fd[4], "rb"); else close(fd[4]);
			
			return pid;
	}//END SWITCH
	
}//END FUNCTION
