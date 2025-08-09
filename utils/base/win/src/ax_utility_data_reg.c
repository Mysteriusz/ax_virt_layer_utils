#include "ax_utility.h"

static AXSTATUS ax_open_data_root_reg(
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

	if (result != ERROR_SUCCESS){
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

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	RegCloseKey(buffer);

	root->location = lbuffer;
	root->location_size = sizeof(HKEY);
	root->type = REGISTRY;

	return AX_SUCCESS;
}
static AXSTATUS ax_get_data_reg(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	wchar_t* buffer = NULL;
	size_t buffer_size = 0;

	LRESULT result = 0;

	if (node->name == NULL 
	|| root->location == NULL) return AX_INVALID_ARGUMENT;

	result = RegQueryValueExW(
		root->location,
		node->name,
		NULL,
		NULL,
		(char*)buffer,
		(uint32_t*)&buffer_size
	); 		
	
	buffer = malloc(buffer_size);

	result = RegQueryValueExW(
		root->location,
		node->name,
		NULL,
		NULL,
		(char*)buffer,
		(uint32_t*)&buffer_size
	); 			

	if (result != ERROR_SUCCESS){
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
	|| node == NULL) return AX_INVALID_ARGUMENT;

	LRESULT result = RegSetValueExW(
		root->location,
		node->name,
		0,
		*((uint32_t*)node->context),
		node->value,
		node->value_size
	); 

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	return AX_SUCCESS;
}

