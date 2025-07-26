#include "install.h"

AXSTATUS ax_setup_i(
	AX_DATA_NODE* config_values,
	unsigned int config_value_count
){
	AX_DATA_ROOT config_root;
	ax_get_data_root(&config_root);

	if (config_values == NULL){
		config_values = (AX_DATA_NODE*)ax_default_data_nodes;	
		config_value_count = AX_DEFAULT_DATA_NODE_COUNT;
	}

	for (unsigned int i = 0; i < config_value_count; i++){
		ax_set_data(&config_root, &config_values[i], "test", sizeof("test"));
	}

	return AX_SUCCESS;
}

