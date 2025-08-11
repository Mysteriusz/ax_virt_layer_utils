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
#define AX_CONTROL_BIN_FULLNAME 	AX_CONTROL_BIN_NAME L".exe"

#define AX_CONTROL_SERVICE_NAME 	L"AX_VIRTUALIZATION_CONTROL"
#define AX_CONTROL_SERVICE_DISPLAY_NAME L"AX_VIRTUALIZATION_PLATORM_CONTROL"
#define AX_CONTROL_SERVICE_ACCESS 	SERVICE_ALL_ACCESS 
#define AX_CONTROL_SERVICE_TYPE 	SERVICE_WIN32_OWN_PROCESS
#define AX_CONTROL_SERVICE_START 	SERVICE_AUTO_START 
#define AX_CONTROL_SERVICE_CALLS 	SERVICE_ACCEPT_STOP | SERVICE_CONTROL_SHUTDOWN | SERVICE_CONTROL_CONTINUE | SERVICE_CONTROL_PAUSE
#define AX_CONTROL_SERVICE_TIMEOUT 	0x1388 // 5 seconds

/*
 	Set of control codes that are used by the service to trigger actions.
 	These range from 128 (0x80) to 255 (0xff) as indicated by winsvc.h LPHANDLER_FUNCTION_EX.
*/
// Tells the service to try to start the kernel driver
#define AX_CONTROL_CODE_START		0x80
// Tells the service to try to stop the kernel driver
#define AX_CONTROL_CODE_STOP		0x81

#if defined(AX_UM)

AXSTATUS ax_control_setup_i(
	AX_IN_OPT const AX_DATA_ROOT*	root
);

#endif // defined(AX_UM)

#endif // !defined(AX_CONTROL_INT)

