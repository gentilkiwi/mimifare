#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <conio.h>
typedef unsigned long portTickType;
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
#else
#define TRUE	pdTRUE
#define FALSE	pdFALSE
typedef unsigned long long	UINT64;
typedef unsigned long		UINT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT8;
typedef signed long long	INT64;
typedef signed long			INT32;
typedef signed short		INT16;
typedef signed char			INT8;
typedef unsigned char		BOOLEAN;
typedef unsigned char		UCHAR;
typedef signed char			SCHAR;
typedef unsigned char		BYTE;
typedef float				REAL32;
typedef double				REAL64;
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#endif
#include "acr89.h"
#endif
