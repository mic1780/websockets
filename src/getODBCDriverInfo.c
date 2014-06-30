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

typedef struct STR_BINDING {
	SQLSMALLINT cDisplaySize;
	char * wszBuffer;
	SQLLEN indPtr;
	int fChar;
	struct STR_BINDING *sNext;
} BINDING;

int main(void) {
	/* code usually used in every ODBC application */
	SQLHENV env;
	SQLHDBC dbc;
	SQLHSTMT stmt;
	SQLRETURN ret;
	
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
	SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
	
	/* connect to the data source */
	ret =	SQLDriverConnect(dbc, NULL, "DSN=localdb;", SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
	if (SQL_SUCCEEDED(ret)) {
		printf("Connected\n");
		if (ret == SQL_SUCCESS_WITH_INFO) {
			//printf("Driver reported the following diagnostics\n");
			//extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC)
		}//END IF
		
    SQLCHAR dbms_name[256], dbms_ver[256], tableAccess[256];
    SQLUINTEGER getdata_support;
    SQLUSMALLINT max_concur_act;
    SQLSMALLINT string_len;

    /*
     *  Find something out about the driver.
     */
    SQLGetInfo(dbc, SQL_DBMS_NAME, (SQLPOINTER)dbms_name,
	       sizeof(dbms_name), NULL);
    SQLGetInfo(dbc, SQL_DBMS_VER, (SQLPOINTER)dbms_ver,
	       sizeof(dbms_ver), NULL);
    SQLGetInfo(dbc, SQL_GETDATA_EXTENSIONS, (SQLPOINTER)&getdata_support,
	       0, 0);
    SQLGetInfo(dbc, SQL_MAX_CONCURRENT_ACTIVITIES, &max_concur_act, 0, 0);
    SQLGetInfo(dbc, SQL_ACCESSIBLE_TABLES, &tableAccess, sizeof(tableAccess), NULL);

    printf("DBMS Name: %s\n", dbms_name);
    printf("DBMS Version: %s\n", dbms_ver);
    if (max_concur_act == 0) {
      printf("SQL_MAX_CONCURRENT_ACTIVITIES - no limit or undefined\n");
    } else {
      printf("SQL_MAX_CONCURRENT_ACTIVITIES = %u\n", max_concur_act);
    }
    if (getdata_support & SQL_GD_ANY_ORDER)
      printf("SQLGetData - columns can be retrieved in any order\n");
    else
      printf("SQLGetData - columns must be retrieved in order\n");
    if (getdata_support & SQL_GD_ANY_COLUMN)
      printf("SQLGetData - can retrieve columns before last bound one\n");
    else
      printf("SQLGetData - columns must be retrieved after last bound one\n");
    printf("Tables: %s\n", tableAccess);
		
		//disconnect
		system("read -n1 -r -p \"Press any key to continue...\" key");
		SQLDisconnect(dbc);
	} else {
		fprintf(stderr, "Failed to connect\n");
		//extract_error("SQLDriverConnect", dbc, SQL_HANDLE_DBC);
	}//END IF
	
	SQLAllocHandle(SQL_HANDLE_ENV, dbc, &stmt);
	/* do something with the statement handle e.g. issue sql */
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	
	//free handles
	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, env);
	
	system("read -n1 -r -p \"Press any key to continue...\" key");
	
	return 0;
}//END main
