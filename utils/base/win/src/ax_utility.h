/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_utility.h

	Abstract:
		This module defines default WINDOWS AX Project`s types, constants and functions 
		that provide an interface for programming and interacting with AX environment.

*/

#if !defined(AX_UTILITY_INT)
#define AX_UTILITY_INT

#if defined(AX_UM) 
#include "windows.h"
#elif defined(AX_KM)
#include "ntddk.h"
#endif

#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "assert.h"
#include "pathcch.h"

#if !defined(AX_WINDOWS)
#define AX_WINDOWS
#endif

#define AX_OUT
#define AX_IN
#define AX_IN_OUT

#define AX_OUT_OPT
#define AX_IN_OPT
#define AX_IN_OUT_OPT

#define AX_CACHE_SIZE			0x200
#define AX_DEFAULT_UPDATE_FOLDER_NAME 	L"update\\"

#include "ax_error.h"

/*
	
	Default configuration data interface.

*/
#include "ax_utility_data.h"

/*
	
	Default services interface.

*/
#include "ax_control.h"
#include "ax_driver.h"

#endif // !defined(AX_UTILITY_INT)

