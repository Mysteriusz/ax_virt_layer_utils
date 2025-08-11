/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_driver.h

	Abstract:
		This module defines default WINDOWS AX Project`s driver service types, constants and functions. 

*/

#if !defined(AX_DRIVER_INT)
#define AX_DRIVER_INT

#include "ax_utility.h"

#define AX_DRIVER_BIN_NAME 		L"ax_virt_layer"
#define AX_DRIVER_BIN_FULLNAME 		AX_DRIVER_BIN_NAME L".sys"

#define AX_DRIVER_SERVICE_NAME 		L"AX_VIRTUALIZATION_DRIVER"
#define AX_DRIVER_SERVICE_DISPLAY_NAME 	L"AX_VIRTUALIZATION_PLATFORM"
#define AX_DRIVER_SERVICE_ACCESS 	SERVICE_ALL_ACCESS 
#define AX_DRIVER_SERVICE_TYPE 		SERVICE_KERNEL_DRIVER
#define AX_DRIVER_SERVICE_START 	SERVICE_DEMAND_START

#if defined(AX_UM)

AXSTATUS ax_driver_setup_i(
	AX_IN_OPT const AX_DATA_ROOT*	root
);

#endif // AX_UM

#endif // AX_DRIVER_INT

