#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
//#include "include/constants.h"
#include "include/structs.h"
//#include "include/functions.h"

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
	int frameCount;
	uint16_t len16;
	char frame[10];
	char *reply =	malloc(sizeof(char) * (len + 8));
	
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
		//NOTE: HAVE NOT FULLY CONFIGURED A MESSAGE OF THIS LENGTH (TODO)
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
	
	memcpy(reply, frame, frameCount);
	memcpy(reply + frameCount, s, len);
	
	//printf("sock: %d\ts: %s\tlen: %d\n", sock, s, len);
	if (write(sock, reply, strlen(reply)) <= 0) {
		printf("\n\nWE ARE NOT WRITING!!\n\n");
	} else {
		//printf("we did write\n");
	}//END IF
	
	free(reply);
	reply = NULL;
	
	return NULL;
}//END FUNCTION
