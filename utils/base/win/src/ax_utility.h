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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <pathcch.h>

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

#define AX_UPDATE_FOLDER_NAME 		L"update"

#define AX_CONTROL_BIN_NAME  		L"ax_virt_control" 
#define AX_CONTROL_BIN_FULLNAME 	AX_CONTROL_BIN_NAME L".exe"

#define AX_DRIVER_BIN_NAME 		L"ax_virt_layer"
#define AX_DRIVER_BIN_FULLNAME 		AX_DRIVER_BIN_NAME L".sys"

#include "ax_error.h"
#include "ax_helper.h"

/*
	
	Default configuration data interface.

*/
#include "ax_utility_reader.h"
#include "ax_utility_data.h"

#endif // !defined(AX_UTILITY_INT)

