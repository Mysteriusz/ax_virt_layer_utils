#include "ax_utility_data.h"

// Data type gathering controllers
#include "ax_utility_data_reg.c"

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

	wchar_t* working_directory = ax_load_working_directory();
	size_t working_directory_size = wcslen(working_directory) * sizeof(wchar_t);

	// Setup context for all nodes depending on the data type 
	switch (root->type){
	case REGISTRY:
		bsd->context = malloc(sizeof(uint32_t));
		upd->context = malloc(sizeof(uint32_t));
		dvp->context = malloc(sizeof(uint32_t));
		ctp->context = malloc(sizeof(uint32_t));

		*((uint32_t*)bsd->context) = REG_SZ;
		*((uint32_t*)upd->context) = REG_SZ;
		*((uint32_t*)dvp->context) = REG_SZ;
		*((uint32_t*)ctp->context) = REG_SZ;
		break;
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
	temp_size = working_directory_size; 
	temp = malloc(temp_size);
	swprintf_s(temp, temp_size, L"%ls", working_directory);

	bsd->value = temp;
	bsd->value_size = temp_size;

	//
	// UPDATE DIRECTORY
	//
	temp_size = working_directory_size + sizeof(AX_UPDATE_PATH); 
	temp = malloc(temp_size);
	swprintf_s(temp, temp_size, L"%ls%ls", working_directory, AX_UPDATE_PATH);

	upd->value = temp;
	upd->value_size = temp_size;

	//
	// DRIVER PATH
	//
	temp_size = working_directory_size + sizeof(AX_DRIVER_BIN_FULLNAME); 
	temp = malloc(temp_size);
	swprintf_s(temp, temp_size, L"%ls%ls", working_directory, AX_DRIVER_BIN_FULLNAME);

	dvp->value = temp;
	dvp->value_size = temp_size;

	//
	// CONTROL PATH
	//
	temp_size = working_directory_size + sizeof(AX_CONTROL_BIN_FULLNAME); 
	temp = malloc(temp_size);
	swprintf_s(temp, temp_size, L"%ls%ls", working_directory, AX_CONTROL_BIN_FULLNAME);

	ctp->value = temp;
	ctp->value_size = temp_size;

CLEANUP:
	free(working_directory);

	// Writeback only on success
	if (AX_ERROR(status) == false){
		*node_array = node_buffer;
		*node_count = node_buffer_count;

		return AX_SUCCESS;
	}

	// This will only be executed when function fails in any of the breakpoints

	ax_free_data(bsd);
	ax_free_data(upd);
	ax_free_data(dvp);
	ax_free_data(ctp);
	free(node_buffer);

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
		// Free each node after writing it to the internal buffer.
		ax_free_data(&node_array[i]);

		if (AX_ERROR(status)){
			break;
		}
	}

	// Free array allocated by ax_get_default_data.
	free(node_array);

	// Checking if any of the nodes failed to write.
	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS;
}

AXSTATUS ax_open_data_root(
	AX_OUT const AX_DATA_ROOT* 	root,	
	AX_IN_OPT const AX_DATA_TYPE*	type
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AX_DATA_TYPE data_type = type != NULL ? *type : AX_DEFAULT_DATA_ROOT_TYPE;

	AXSTATUS status = AX_SUCCESS;
	
	switch (data_type){
	case DIRECTORY:
		return AX_NOT_IMPLEMENTED;
	case REGISTRY:
		status = ax_open_data_root_reg((AX_DATA_ROOT*)root);
		break;
	default:
		return AX_INVALID_DATA;
	}

	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS; 
}

/*
 	Main functions
*/

AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

	AXSTATUS status = AX_SUCCESS;
	
	switch (root->type){
	case DIRECTORY:
		return AX_NOT_IMPLEMENTED;
	case REGISTRY:
		status = ax_get_data_reg((AX_DATA_ROOT*)root, node);
		break;
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
	|| node == NULL) return AX_INVALID_ARGUMENT;

	AXSTATUS status = AX_SUCCESS;

	switch (root->type){
	case DIRECTORY:
		return AX_NOT_IMPLEMENTED;
	case REGISTRY:
		status = ax_set_data_reg((AX_DATA_ROOT*)root, (AX_DATA_NODE*)node);
		break;
	default:
		return AX_INVALID_DATA;
	}

	if (AX_ERROR(status)){
		return status;
	}

	return AX_SUCCESS; 
}

void ax_free_root(
	AX_IN_OUT AX_DATA_ROOT*		root
){
	RegCloseKey(root->location);
	memset(root, 0, sizeof(AX_DATA_ROOT));
}
void ax_free_data(
	AX_IN AX_DATA_NODE* 		node
){
	free(node->value);
	free(node->context);
}

