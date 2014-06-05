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
//COMPILE WITH -lodbc32
//THIS FILE IS NOT READY! DO NOT COMPILE UNLESS FINISHED AND TESTED

#include "include/constants.h"
#if __ISWINDOWS__ == 1
#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>
//#include <sqltypes.h>
#include <sqlucode.h>
//#include <odbcinst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

typedef struct STR_BINDING {
	SQLSMALLINT cDisplaySize;
	char * wszBuffer;
	SQLLEN indPtr;
	int fChar;
	struct STR_BINDING *sNext;
} BINDING;

int main(void) {
	SQLHENV env;
	SQLHSTMT stmt;
	SQLRETURN ret;
	char driver[256];
	char attr[256];
	SQLSMALLINT driver_ret;
	SQLSMALLINT attr_ret;
	SQLUSMALLINT direction;
	
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	
	direction = SQL_FETCH_FIRST;
	while(SQL_SUCCEEDED(ret = SQLDrivers(env, direction,
													driver, sizeof(driver), &driver_ret,
													attr, sizeof(attr), &attr_ret)
							)
	) {
		direction = SQL_FETCH_NEXT;
		printf("%s - %s\n", driver, attr);
		if (ret == SQL_SUCCESS_WITH_INFO) {
			printf("\tdata truncation\n");
		}//END IF
	}//END WHILE LOOP
	
	//if (__ISWINDOWS__) {
		//system("pause");
		//system("pause");
	//} else {
		system("read -n1 -r -p \"Press any key to continue...\" key");
	//}//END IF
	
	return 0;
}//END main
