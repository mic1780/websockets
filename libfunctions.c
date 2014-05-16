//NO RELATION TO libfunctions.dll
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/structs.h"

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
