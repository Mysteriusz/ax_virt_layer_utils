#include "upd_action.h"

AXSTATUS upd_action_install(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	// Get root of the configuration data
	AX_DATA_ROOT root;
	ax_get_data_root(&root);

	// Get default configuration nodes
	AX_DATA_NODE* default_data = ax_get_data_node_d();
	// Write all default data nodes to the root
	for (uint32_t i = 0; i < AX_DATA_NODE_COUNT_D; i++){
		ax_set_data(&root, &default_data[i]);
	}

	ax_free_root(&root);

	// Setup control and the driver using their interface
	ax_control_setup_i();
	ax_driver_setup_i();

	return AX_SUCCESS;
}
AXSTATUS upd_action_update(
	AX_IN_OPT void*			stack // STACK NOT USED
){
	return AX_SUCCESS;
}
