/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		install.h

	Abstract:
		This module defines install command types, constants and functions 
		which is a part of the update.exe utility.

*/

#include "D:/ax_project/ax_virt_layer_utils/utils/base/src/ax_utility.h"

/*
 	Function used for handling the setup of install command.

	IMPORTANT NOTICE:

	If config_values is NULL setup will use default nodes defined in ax_utility.h
*/
AXSTATUS ax_setup_i(
	AX_IN_OPT AX_DATA_NODE 		*config_values,
	AX_IN uint32_t 			config_count 
);

typedef struct{
	wchar_t* 	name;
	wchar_t* 	bin_path;

	// Windows exclusive service data
#if defined(AX_WINDOWS)
	wchar_t* 	sc_display_name;	// for CreateServiceW -> lpDisplayName	
	uint32_t	sc_access; 		// for CreateServiceW -> dwDesiredAccess 
	uint32_t	sc_type; 		// for CreateServiceW -> dwServiceType 
	uint32_t	sc_start; 		// for CreateServiceW -> dwStartType
#endif

} AX_DRIVER_DATA_I;

#if defined(AX_WINDOWS)

// AX Virtualization data 
#define AX_VIRT_LAYER_DATA_I(bp) 					\
	(AX_DRIVER_DATA_I){ 						\
		.name=			AX_SERVICE_NAME, 		\
		.bin_path=		(wchar_t*)bp, 			\
		.sc_display_name= 	AX_SERVICE_DISPLAY_NAME, 	\
		.sc_access=		AX_SERVICE_ACCESS, 		\
		.sc_type=		AX_SERVICE_TYPE, 		\
		.sc_start=		AX_SERVICE_START, 		\
	}
// AX Control data
#define AX_CONTROL_DATA_I(bp) \
	(AX_DRIVER_DATA_I){ \
		.name=			AX_CSERVICE_NAME, 		\
		.bin_path=		(wchar_t*)bp, 			\
		.sc_display_name=	AX_CSERVICE_DISPLAY_NAME, 	\
		.sc_access=		AX_CSERVICE_ACCESS, 		\
		.sc_type=		AX_CSERVICE_TYPE, 		\
		.sc_start=		AX_CSERVICE_START, 		\
	} 

#endif

/*
 	Function used for handling the setup of default runtime of the driver and updater.
*/
AXSTATUS ax_driver_i(
	AX_IN AX_DRIVER_DATA_I 		*data_i	
);

