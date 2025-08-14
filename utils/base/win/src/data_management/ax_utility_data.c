#include "ax_utility_data.h"

// Data type gathering handlers
#include "ax_utility_data_dir.c" // DATA_TYPE_DIRECTORY
//#include "ax_utility_data_file.c" // DATA_TYPE_FILE
#include "ax_utility_data_reg.c" // DATA_TYPE_REGISTRY

/*

 	Main functions
 
*/

AXSTATUS ax_open_data_root(
	AX_OUT const AX_DATA_ROOT* 	root,	
	AX_IN_OPT const AX_DATA_TYPE*	type,
	AX_IN_OPT const void*		context
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	AX_DATA_TYPE data_type = type != NULL ? *type : AX_DEFAULT_DATA_ROOT_TYPE;

	switch (data_type){
	case DATA_TYPE_DIRECTORY:
		status = _ax_open_data_root_dir((AX_DATA_ROOT*)root, (wchar_t*)context);
		_ax_print_w(L"kurwa");
		break;
	case DATA_TYPE_FILE:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_REGISTRY:
		status = _ax_open_data_root_reg((AX_DATA_ROOT*)root);
		break;
	case DATA_TYPE_SERVER:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_SECURE_SERVER:
		return AX_NOT_IMPLEMENTED;
	default:
		return AX_INVALID_DATA;
	}

	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS;
}

AXSTATUS ax_get_default_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_OUT AX_DATA_NODE**		node_array,
	AX_OUT uint32_t*		node_count
){
	if (root == NULL
		|| node_array == NULL
		|| node_count == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	uint32_t node_buffer_count = AX_DEFAULT_NODE_COUNT;
	AX_DATA_NODE* node_buffer = malloc(sizeof(AX_DATA_NODE) * node_buffer_count);

	AX_DATA_NODE* bsd = &node_buffer[0];
	memcpy(bsd, &AX_DATA_NODE_BSD(root->type), sizeof(AX_DATA_NODE));
	AX_DATA_NODE* upd = &node_buffer[1];
	memcpy(upd, &AX_DATA_NODE_UPD(root->type), sizeof(AX_DATA_NODE));
	AX_DATA_NODE* dvp = &node_buffer[2];
	memcpy(dvp, &AX_DATA_NODE_DVP(root->type), sizeof(AX_DATA_NODE));
	AX_DATA_NODE* ctp = &node_buffer[3];
	memcpy(ctp, &AX_DATA_NODE_CTP(root->type), sizeof(AX_DATA_NODE));

	wchar_t* working_directory = _ax_load_working_directory();

	// Setup context for all nodes depending on the data type 
	switch (root->type){
	case DATA_TYPE_DIRECTORY:
		PathAllocCombine(
			root->location,
			bsd->name,
			PATHCCH_ALLOW_LONG_PATHS, 
			(wchar_t**)&bsd->context
		);
		PathAllocCombine(
			root->location,
			upd->name,
			PATHCCH_ALLOW_LONG_PATHS, 
			(wchar_t**)&upd->context
		);
		PathAllocCombine(
			root->location,
			dvp->name,
			PATHCCH_ALLOW_LONG_PATHS, 
			(wchar_t**)&dvp->context
		);
		PathAllocCombine(
			root->location,
			ctp->name,
			PATHCCH_ALLOW_LONG_PATHS, 
			(wchar_t**)&ctp->context
		);

		break;
	case DATA_TYPE_FILE:{
		return AX_NOT_IMPLEMENTED;
	}
	case DATA_TYPE_REGISTRY:
		bsd->context = malloc(sizeof(uint32_t));
		upd->context = malloc(sizeof(uint32_t));
		dvp->context = malloc(sizeof(uint32_t));
		ctp->context = malloc(sizeof(uint32_t));

		*((uint32_t*)bsd->context) = REG_SZ;
		*((uint32_t*)upd->context) = REG_SZ;
		*((uint32_t*)dvp->context) = REG_SZ;
		*((uint32_t*)ctp->context) = REG_SZ;
		break;
	case DATA_TYPE_SERVER:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_SECURE_SERVER:
		return AX_NOT_IMPLEMENTED;
	default:
		status = AX_NOT_IMPLEMENTED;
		goto CLEANUP;
	}

	wchar_t* temp = NULL;
	size_t temp_size = 0;

	// Setup all data nodes

	//
	// BASE DIRECTORY
	//
	PathAllocCombine(
		working_directory,
		L"", 
		PATHCCH_ALLOW_LONG_PATHS, 
		&temp
	);

	bsd->value = temp;
	bsd->value_size = (wcslen(temp) + 1) * sizeof(wchar_t);

	//
	// UPDATE DIRECTORY
	//
	PathAllocCombine(
		working_directory,
		AX_UPDATE_FOLDER_NAME, 
		PATHCCH_ALLOW_LONG_PATHS, 
		&temp
	);

	upd->value = temp;
	upd->value_size = (wcslen(temp) + 1) * sizeof(wchar_t);

	//
	// DRIVER PATH
	//
	PathAllocCombine(
		working_directory,
		AX_DRIVER_BIN_FULLNAME, 
		PATHCCH_ALLOW_LONG_PATHS, 
		&temp
	);

	dvp->value = temp;
	dvp->value_size = (wcslen(temp) + 1) * sizeof(wchar_t);

	//
	// CONTROL PATH
	//
	PathAllocCombine(
		working_directory,
		AX_CONTROL_BIN_FULLNAME, 
		PATHCCH_ALLOW_LONG_PATHS, 
		&temp
	);

	ctp->value = temp;
	ctp->value_size = (wcslen(temp) + 1) * sizeof(wchar_t);

CLEANUP:
	free(working_directory);

	// Writeback only on success
	if (AX_ERROR(status) == false){
		*node_array = node_buffer;
		*node_count = node_buffer_count;

		return AX_SUCCESS;
	}

	// This will only be executed when function fails in any of the breakpoints

	__debugbreak();
	ax_free_data_array(node_buffer, node_buffer_count);

	return status;
}
AXSTATUS ax_set_default_data(
	AX_IN const AX_DATA_ROOT*	root
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	AX_DATA_NODE* node_array = NULL;
	uint32_t node_array_count = 0;
	status = ax_get_default_data(root, &node_array, &node_array_count);

	if (AX_ERROR(status)){
		return status;
	}

	for (uint32_t i = 0; i < node_array_count; i++){
		status = ax_set_data(root, &node_array[i]);

		if (AX_ERROR(status)){
			break;
		}
	}

	// Free array allocated by ax_get_default_data.
	ax_free_data_array(node_array, node_array_count);

	// Checking if any of the nodes failed to write.
	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS;
}

AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL 
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;
	
	switch (root->type){
	case DATA_TYPE_DIRECTORY:
		status = _ax_get_data_dir((AX_DATA_ROOT*)root, node);
		break;
	case DATA_TYPE_FILE:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_REGISTRY:
		status = _ax_get_data_reg((AX_DATA_ROOT*)root, node);
		break;
	case DATA_TYPE_SERVER:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_SECURE_SERVER:
		return AX_NOT_IMPLEMENTED;
	default:
		return AX_INVALID_DATA;
	}

	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS; 
}

AXSTATUS ax_set_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN const AX_DATA_NODE*	node
){
	if (root == NULL 
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	switch (root->type){
	case DATA_TYPE_DIRECTORY:
		status = _ax_set_data_dir((AX_DATA_ROOT*)root, (AX_DATA_NODE*)node);
		break;
	case DATA_TYPE_FILE:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_REGISTRY:
		status = _ax_set_data_reg((AX_DATA_ROOT*)root, (AX_DATA_NODE*)node);
		break;
	case DATA_TYPE_SERVER:
		return AX_NOT_IMPLEMENTED;
	case DATA_TYPE_SECURE_SERVER:
		return AX_NOT_IMPLEMENTED;
	default:
		return AX_INVALID_DATA;
	}

	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS; 
}

void ax_free_root(
	AX_IN AX_DATA_ROOT*		root
){
	if (root == NULL){
		return;
	}

	switch (root->type){
	case DATA_TYPE_DIRECTORY:
		if (root->location) free(root->location);
	case DATA_TYPE_FILE:
		_ax_log_status(AX_NOT_IMPLEMENTED, false, NULL, L"DATA_TYPE_FILE ax_free_root not implemented.");
		return;
	case DATA_TYPE_REGISTRY:
		if (root->location) RegCloseKey(root->location); // Close registry handle 
	case DATA_TYPE_SERVER:
		_ax_log_status(AX_NOT_IMPLEMENTED, false, NULL, L"DATA_TYPE_SERVER ax_free_root not implemented.");
		return;
	case DATA_TYPE_SECURE_SERVER:
		_ax_log_status(AX_NOT_IMPLEMENTED, false, NULL, L"DATA_TYPE_SECURE_SERVER ax_free_root not implemented.");
		return;
	default:
		return;
	}

	return;
}
void ax_free_data(
	AX_IN AX_DATA_NODE* 		node
){
	if (node == NULL){
		return;
	}

	if (node->value) free(node->value);
	if (node->context) free(node->context);

	return;
}
void ax_free_data_array(
	AX_IN AX_DATA_NODE*	 	node_array,
	AX_IN uint32_t			node_count
){	
	if (node_array == NULL){
		return;
	}

	for (uint32_t i = 0; i < node_count; i++){
		ax_free_data(&node_array[i]);
	}

	free(node_array);

	return;
}

