#include "ax_utility.h"

/*

   	AWAITING CODE REFACTOR!!!!!

*/

static AXSTATUS _ax_open_data_root_reg(
	AX_IN_OUT AX_DATA_ROOT*			root,
	AX_IN wchar_t*				key
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	wchar_t** split = NULL;
	size_t split_size = 0;
	struct AX_READER_SETTINGS settings = {
		.label = NULL,
		.char_set = AX_PATH_CHAR_SET
	};

	UNREFERENCED_PARAMETER(key);
	ax_split_text(L"path\\to\\something", &settings, &split, &split_size);

	LRESULT result = NO_ERROR;
	HKEY buffer = NULL;
	HKEY lbuffer = NULL;

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
static AXSTATUS _ax_get_data_reg(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL
	|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_REGISTRY
	|| root->location == NULL
	|| node->name == NULL){
		return AX_INVALID_DATA;
	}

	LRESULT result = NO_ERROR;

	wchar_t* buffer = NULL;
	uint32_t* buffer_context = NULL;
	size_t buffer_size = 0;

	result = RegQueryValueExW(
		root->location,
		node->name,
		NULL,
		NULL,
		(LPBYTE)buffer,
		(LPDWORD)&buffer_size
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
	 	(LPDWORD)buffer_context,
		(LPBYTE)buffer,
		(LPDWORD)&buffer_size
	); 			

	if (result != NO_ERROR){
		free(buffer);
		return result | AX_STATUS_LRESULT;
	}

	node->value = buffer;
	node->value_size = buffer_size;

	return AX_SUCCESS;
}
static AXSTATUS _ax_set_data_reg(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN AX_DATA_NODE*		node
){
	if (root == NULL 
	|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_REGISTRY
	|| root->location == NULL
	|| node->name == NULL
	|| node->value == NULL){
		return AX_INVALID_DATA;
	}

	if (node->context == NULL){
		return AX_UNKNOWN_CONTEXT;
	}

	LRESULT result = NO_ERROR; 

	result = RegSetValueExW(
		root->location,
		node->name,
		0,
		*(uint32_t*)node->context,
		node->value,
		(DWORD)node->value_size
	); 

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT;
	}

	return AX_SUCCESS;
}

