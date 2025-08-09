#include "upd_action.h"

AXSTATUS upd_action_install(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	AXSTATUS status = AX_SUCCESS;

	// Get root of the configuration data
	const AX_DATA_ROOT root;
	ax_open_data_root(&root, NULL);

	// Set default configuration nodes
	ax_set_default_data(&root);

	ax_free_root((AX_DATA_ROOT*)&root);

	// Setup control and the driver using their interface
	status = ax_control_setup_i();
	if (AX_ERROR(status)){
		ax_log_status(status, true, NULL, NULL);
	}
	
	status = ax_driver_setup_i();
	if (AX_ERROR(status)){
		ax_log_status(status, true, NULL, NULL);
	}

	return AX_SUCCESS;
}
AXSTATUS upd_action_update(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	return AX_SUCCESS;
}
