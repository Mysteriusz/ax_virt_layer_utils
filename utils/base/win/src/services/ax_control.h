/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_control.h

	Abstract:
		This module defines default WINDOWS AX Project`s control service types, constants and functions. 

*/

#if !defined(AX_CONTROL_INT)
#define AX_CONTROL_INT

#include "ax_utility.h"

#define AX_CONTROL_BIN_NAME  		L"ax_virt_control" 
#define AX_CONTROL_BIN_FULLNAME 	L"\\" AX_CONTROL_BIN_NAME L".exe"

#define AX_CONTROL_SERVICE_NAME 	L"AX_VIRTUALIZATION_CONTROL"
#define AX_CONTROL_SERVICE_DISPLAY_NAME L"AX_VIRTUALIZATION_PLATORM_CONTROL"
#define AX_CONTROL_SERVICE_ACCESS 	SERVICE_ALL_ACCESS 
#define AX_CONTROL_SERVICE_TYPE 	SERVICE_WIN32_OWN_PROCESS
#define AX_CONTROL_SERVICE_START 	SERVICE_AUTO_START
#define AX_CONTROL_SERVICE_CALLS 	SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PRESHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;


#if defined(AX_UM)

AXSTATUS ax_control_setup_i(
	void
);

#endif // AX_UM

#endif // AX_CONTROL_INT

