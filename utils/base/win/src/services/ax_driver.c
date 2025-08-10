#include "ax_driver.h"

AXSTATUS ax_driver_setup_i(
	void
){
	AXSTATUS status = 0;

	SC_HANDLE sc = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);  

	if (sc == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	AX_DATA_ROOT root;
	ax_open_data_root(&root, NULL, NULL);

	AX_DATA_NODE bin_path = AX_DATA_NODE_DVP(root.type);
	ax_get_data(&root, &bin_path);

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
	ax_free_root(&root);
	CloseServiceHandle(sc);

	if (driver_service == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	CloseServiceHandle(driver_service);

	return AX_SUCCESS;
}
