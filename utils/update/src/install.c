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

	AX_DRIVER_I_DATA ax_virt_layer_data = (AX_DRIVER_I_DATA){
		.name=AX_SERVICE_NAME,
		.bin_path=dvp.value,
#if defined(AX_WINDOWS)
		.sc_display_name=AX_SERVICE_DISPLAY_NAME,
		.sc_access=AX_SERVICE_ACCESS,
		.sc_type=AX_SERVICE_TYPE,
		.sc_start=AX_SERVICE_START,
#endif
	};
	ax_driver_i(&ax_virt_layer_data);

	ax_free_data(&dvp);
	return AX_SUCCESS;
}

AXSTATUS ax_driver_i(
	AX_IN AX_DRIVER_I_DATA* data_i	
){
	if (data_i == NULL) return AX_INVALID_ARGUMENT; 

#if defined(AX_WINDOWS)
	SC_HANDLE sch = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (sch == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	sch = CreateServiceW(sch, data_i->name, data_i->sc_display_name, data_i->sc_access, data_i->sc_type, data_i->sc_start, SERVICE_ERROR_CRITICAL, data_i->bin_path, NULL, NULL, NULL, NULL, NULL);

	CloseServiceHandle(sch);
#endif

	return AX_SUCCESS;	
}

