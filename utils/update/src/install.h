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
AXSTATUS ax_driver_i(
	AX_IN char* cmd
);

