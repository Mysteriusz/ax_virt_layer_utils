#include "install.h"
#include "stdio.h"

AXSTATUS ax_setup_i(
	AX_DATA_NODE* config_values,
	unsigned int config_value_count
){
	AX_DATA_ROOT config_root;
	ax_get_data_root(&config_root);

	if (config_values == NULL){
		config_values = ax_get_default_data_nodes();	
		config_value_count = AX_DEFAULT_DATA_NODE_COUNT;
	}

	for (unsigned int i = 0; i < config_value_count; i++){
		ax_set_data(&config_root, &config_values[i]);
	}

	AX_DATA_NODE dvp = AX_DATA_NODE_DVP;   
	ax_get_data(&config_root, &dvp);

	size_t driver_cmd_size = sizeof(AX_SERVICE_CMD) + strlen(dvp.value);
	char* driver_cmd = malloc(driver_cmd_size); 
	strcpy_s(driver_cmd, driver_cmd_size, AX_SERVICE_CMD);
	strcat_s(driver_cmd, driver_cmd_size, dvp.value);

	ax_driver_i(driver_cmd);

	ax_free_data(&dvp);
	free(driver_cmd);
	return AX_SUCCESS;
}
AXSTATUS ax_driver_i(
	AX_IN char* cmd
){
	printf(cmd);
#if defined(AX_WINDOWS)
	ShellExecuteA(NULL, NULL, "powershell.exe", cmd, NULL, SW_HIDE);
#endif

	return AX_SUCCESS;
}

