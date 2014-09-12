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

#include "../include/constants.h"
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

void testPrint(int line, char * func) {
	printf("Function %s is on line %d.\n", func, line);
}//END VOID

void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type);

int main(void) {
	/* code usually used in every ODBC application */
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret;
	SQLSMALLINT columns;
	int row = 0;
	
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	
	/* connect to the data source */
	ret =	SQLDriverConnect(dbc, NULL, "DSN=local;", SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
	if (SQL_SUCCEEDED(ret)) {
		printf("Connected\n");
		if (ret == SQL_SUCCESS_WITH_INFO) {
			printf("Driver reported the following diagnostics\n");
			extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
		}//END IF
		
		SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
		
		//SQLExecDirect(stmt, "SELECT TOP 1000 * FROM tbl_Users", SQL_NTS);
		//SQLTables(stmt, NULL, 0, NULL, 0, NULL, 0, (SQLCHAR*)"TABLE", SQL_NTS);
		SQLColumns(stmt, NULL, 0, NULL, 0, (SQLCHAR*)"tbl_Users", SQL_NTS, NULL, 0);
		SQLNumResultCols(stmt, &columns);
		while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
			SQLUSMALLINT i;
			printf("Row %d\n", row++);
			/* Loop through the columns */
			for (i = 1; i <= columns; i++) {
				SQLINTEGER indicator;
				char buf[512];
				/* retrieve column data as a string */
				ret = SQLGetData(stmt, i, SQL_C_CHAR,
				       buf, sizeof(buf), &indicator);
				if (SQL_SUCCEEDED(ret)) {
					/* Handle null columns */
					if (indicator == SQL_NULL_DATA) strcpy(buf, "NULL");
						printf("  Column %u : %s\n", i, buf);
				}//END IF
			}//END FOR LOOP
		}//END WHILE LOOP
		/* do something with the statement handle e.g. issue sql */
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		
		
		//disconnect
		SQLDisconnect(dbc);
	} else {
		fprintf(stderr, "Failed to connect\n");
		extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
	}//END IF
	
	//free handles
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);
	
	system("read -n1 -r -p \"Press any key to continue...\" key");
	
	return 0;
}//END main

void extract_error(
    char *fn,
    SQLHANDLE handle,
    SQLSMALLINT type)
{
    SQLINTEGER	 i = 0;
    SQLINTEGER	 native;
    SQLCHAR	 state[ 7 ];
    SQLCHAR	 text[256];
    SQLSMALLINT	 len;
    SQLRETURN	 ret;

    fprintf(stderr,
            "\n"
            "The driver reported the following diagnostics whilst running "
            "%s\n\n",
            fn);

    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                            sizeof(text), &len );
        if (SQL_SUCCEEDED(ret))
            printf("%s:%ld:%ld:%s\n", state, i, native, text);
    }
    while( ret == SQL_SUCCESS );
}
