#include "ax_driver.h"

AXSTATUS ax_driver_setup_i(
	AX_IN_OPT const AX_DATA_ROOT*	root
){
	AXSTATUS status = 0;

	SC_HANDLE sc = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);  

	if (sc == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	bool local_root = root == NULL ? true : false;

	const AX_DATA_ROOT* data_root = root;
	if (local_root){
		status = ax_open_data_root(data_root, NULL, NULL);

		if (AX_ERROR(status)){
			return status;
		}
	}

	AX_DATA_NODE bin_path = AX_DATA_NODE_DVP(data_root->type);
	status = ax_get_data(data_root, &bin_path);

	if (AX_ERROR(status)){
		return status;
	}

	SC_HANDLE driver_service = CreateServiceW(sc,
		AX_DRIVER_SERVICE_NAME,
		AX_DRIVER_SERVICE_DISPLAY_NAME,
		AX_DRIVER_SERVICE_ACCESS,
		AX_DRIVER_SERVICE_TYPE,
		AX_DRIVER_SERVICE_START,
		SERVICE_ERROR_CRITICAL,
		bin_path.value,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	ax_free_data(&bin_path);
	ax_free_root((AX_DATA_ROOT*)root);
	CloseServiceHandle(sc);

	if (driver_service == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	CloseServiceHandle(driver_service);

	return AX_SUCCESS;
}
