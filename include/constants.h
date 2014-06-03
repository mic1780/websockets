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
#ifndef CONSTANTS
#define CONSTANTS

#define TRUE 1
#define FALSE 0

#define CONNECTING 0
#define OPEN 1
#define CLOSING 2
#define CLOSED 3
#define NUM_OF_CLIENTS 600

#define IP_OCTET_1 127
#define IP_OCTET_2 0
#define IP_OCTET_3 0
#define IP_OCTET_4 1
#define CONNECTION_PORT 1780


//#if defined(_X86_) || defined(__x86_64__)
#if defined(_X86_) || defined(__x86_64__)
#define __ISWINDOWS__ 1
#else
#define __ISWINDOWS__ 0
#endif

#endif
