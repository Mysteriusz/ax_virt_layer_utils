#include "D:/ax_project/ax_virt_layer_utils/utils/base/src/ax_utility.h"

/*
 	Function used for handling the setup of install command.

	IMPORTANT NOTICE:

	If config_values is NULL setup will use default nodes defined in ax_utility.h
*/
AXSTATUS ax_setup_i(
	AX_IN_OPT AX_DATA_NODE* config_values,
	AX_IN unsigned int config_values_count
);

typedef struct{
	wchar_t* name;
	wchar_t* bin_path;
	// Windows service data
#if defined(AX_WINDOWS)
	wchar_t* sc_display_name;
	unsigned long sc_access;
	unsigned long sc_type;
	unsigned long sc_start;
#endif
} AX_DRIVER_I_DATA;

AXSTATUS ax_driver_i(
	AX_IN AX_DRIVER_I_DATA* data_i	
);

