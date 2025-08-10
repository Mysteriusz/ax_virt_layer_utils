#include "ax_control.h"

AXSTATUS ax_control_setup_i(
	void
){
	AXSTATUS status = 0;

	SC_HANDLE sc = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);  

	if (sc == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	AX_DATA_ROOT root;
	ax_open_data_root(&root, NULL, NULL);

	AX_DATA_NODE bin_path = AX_DATA_NODE_CTP(root.type);
	ax_get_data(&root, &bin_path);

	SC_HANDLE control_service = CreateServiceW(sc,
		AX_CONTROL_SERVICE_NAME,
		AX_CONTROL_SERVICE_DISPLAY_NAME,
		AX_CONTROL_SERVICE_ACCESS,
		AX_CONTROL_SERVICE_TYPE,
		AX_CONTROL_SERVICE_START,
		SERVICE_ERROR_NORMAL,
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

	if (control_service == NULL){
		return GetLastError() | AX_STATUS_LERROR;
	}

	CloseServiceHandle(control_service);

	return AX_SUCCESS;
}

