#include "ax_utility.h"

static AXSTATUS _ax_open_data_root_reg(
	AX_IN_OUT AX_DATA_ROOT*		root
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	LRESULT result = 0;
	HKEY buffer;

	result = RegOpenKeyEx(
		AX_DATA_ROOT_HKEY,
		AX_DATA_ROOT_SUBKEY,
		0, 
		KEY_CREATE_SUB_KEY, 
		&buffer
	);

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT;
	}

	HKEY lbuffer;
	result = RegCreateKeyExW(
		buffer,
		AX_DATA_ROOT_NAME,
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&lbuffer,
		NULL
	);

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT;
	}

	RegCloseKey(buffer);

	root->location = lbuffer;
	root->location_size = sizeof(HKEY);
	root->type = DATA_TYPE_REGISTRY;

	return AX_SUCCESS;
}
static AXSTATUS ax_get_data_reg(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_REGISTRY){
		return AX_INVALID_DATA;
	}

	wchar_t* buffer = NULL;
	uint32_t* buffer_context = NULL;
	size_t buffer_size = 0;

	LRESULT result = NO_ERROR;

	result = RegQueryValueExW(
		root->location,
		node->name,
		NULL,
		NULL,
		(char*)buffer,
		(uint32_t*)&buffer_size
	); 		

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT;
	}
	
	buffer = malloc(buffer_size);
	buffer_context = malloc(sizeof(uint32_t));

	result = RegQueryValueExW(
		root->location,
		node->name,
		NULL,
	 	buffer_context,
		(char*)buffer,
		(uint32_t*)&buffer_size
	); 			

	if (result != NO_ERROR){
		free(buffer);
		return result | AX_STATUS_LRESULT;
	}

	node->value = buffer;
	node->value_size = buffer_size;

	return AX_SUCCESS;
}
static AXSTATUS ax_set_data_reg(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN AX_DATA_NODE*		node
){
	if (root == NULL 
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_REGISTRY){
		return AX_INVALID_DATA;
	}

	if (node->context == NULL){
		return AX_UNKNOWN_CONTEXT;
	}

	LRESULT result = RegSetValueExW(
		root->location,
		node->name,
		0,
		*(uint32_t*)node->context,
		node->value,
		node->value_size
	); 

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT;
	}

	return AX_SUCCESS;
}

