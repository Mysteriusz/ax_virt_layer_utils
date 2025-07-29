#include "install.h"
#include "stdio.h"

AXSTATUS ax_setup_i(
	AX_DATA_NODE *config_values,
	uint32_t config_value_count
){
	AX_DATA_ROOT config_root;
	ax_get_data_root(&config_root);

	if (config_values == NULL){
		config_values = ax_get_data_node_d();
		config_value_count = AX_DATA_NODE_COUNT_D;
	}

	for (uint32_t i = 0; i < config_value_count; i++){
		ax_set_data(&config_root, &config_values[i]);
	}

	AX_DATA_NODE dvp = AX_DATA_NODE_DVP(NULL);   
	ax_get_data(&config_root, &dvp);
	AX_DATA_NODE ctp = AX_DATA_NODE_CTP(NULL);   
	ax_get_data(&config_root, &ctp);
	
	ax_driver_i(&AX_VIRT_LAYER_DATA_I(dvp.value));
	ax_driver_i(&AX_CONTROL_DATA_I(ctp.value));

	ax_free_data(&dvp);
	ax_free_data(&ctp);
	return AX_SUCCESS;
}

AXSTATUS ax_driver_i(
	AX_IN AX_DRIVER_DATA_I *data_i	
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

