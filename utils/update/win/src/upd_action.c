#include "upd_action.h"

// TODO: Stack structure that lets user to use different configuration type (AX_DATA_TYPE) 
AXSTATUS upd_action_install(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	AXSTATUS status = AX_SUCCESS;

	// Get root of the configuration data
	const AX_DATA_ROOT root;
	AX_DATA_TYPE root_type = DATA_TYPE_DIRECTORY;
	status = ax_open_data_root(&root, &root_type, NULL);
	if (AX_ERROR(status)){
		ax_log_status(
			status,
			true, 
			NULL, 
			L"CRITICAL: Reading root of the default configuration data failed."
		);
		__debugbreak();

		return status;
	}

	// Set default configuration nodes
	status = ax_set_default_data(&root);
	if (AX_ERROR(status)){
		ax_log_status(
			status,
			true, 
			NULL, 
			L"CRITICAL: Writing default configuration data failed."
		);
		__debugbreak();

		return status;
	}

	AX_DATA_NODE temp = AX_DATA_NODE_DVP(DATA_TYPE_DIRECTORY);
	status = ax_get_data(&root, &temp);

	printf("%ls", (wchar_t*)temp.context);
	ax_free_data(&temp);

	//ax_free_root((AX_DATA_ROOT*)&root);

	// Setup control and the driver using their interface
	/*status = ax_control_setup_i();
	if (AX_ERROR(status)){
		ax_log_status(status, true, NULL, NULL);
	}
	
	status = ax_driver_setup_i();
	if (AX_ERROR(status)){
		ax_log_status(status, true, NULL, NULL);
	}*/

	return AX_SUCCESS;
}
AXSTATUS upd_action_update(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	return AX_SUCCESS;
}
