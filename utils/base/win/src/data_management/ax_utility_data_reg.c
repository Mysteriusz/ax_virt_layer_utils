#include "ax_utility.h"

static AXSTATUS _ax_create_data_root_reg(
	AX_IN wchar_t*				path,
	AX_IN HKEY				system_root,
	AX_OUT HKEY*				new_key
){
	if (path == NULL
	|| system_root == NULL
	|| new_key == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;
	LSTATUS result = NO_ERROR;	

	HKEY system_buffer = NULL;

	result = RegOpenKeyExW(
		system_root,
		NULL,
		0,
		KEY_CREATE_SUB_KEY,
		&system_buffer
	);

	if (result != NO_ERROR){
		return result | AX_STATUS_LRESULT; 
	}
	
	wchar_t** path_array = NULL;
	size_t path_array_size = 0;
	struct AX_READER_SETTINGS path_settings = {
		.label = NULL,
		.char_set = AX_PATH_CHAR_SET
	};

	status = ax_split_text(path, &path_settings, &path_array, &path_array_size);
	if (AX_ERROR(status)){
		return status;
	}

	HKEY current_buffer = NULL;	
	HKEY previous_buffer = system_buffer;	
	for (uint32_t i = 0; i < path_array_size; i++){
		result = RegCreateKeyExW(
			previous_buffer,
			path_array[i],
			0,
			NULL,
			0,
			KEY_CREATE_SUB_KEY | KEY_READ | KEY_WRITE,
			NULL,
			&current_buffer,
			NULL
		);

		if (i < path_array_size - 1){
			RegCloseKey(previous_buffer);
			previous_buffer = current_buffer;
		}

		if (result != NO_ERROR){
			_ax_free_array(path_array, path_array_size);
			return result | AX_STATUS_LRESULT; 
		}
	}
	_ax_free_array(path_array, path_array_size);
	
	*new_key = current_buffer;

	return AX_SUCCESS;
}

static AXSTATUS _ax_open_data_root_reg(
	AX_IN_OUT AX_DATA_ROOT*			root,
	AX_IN_OPT wchar_t*			path
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	HKEY* buffer = malloc(sizeof(HKEY));

	wchar_t* path_buffer =
		(path != NULL)
		? path
		: AX_DEFAULT_DATA_ROOT_REG; 	 

	status = _ax_create_data_root_reg(path_buffer, AX_DEFAULT_SYSTEM_ROOT_REG, buffer);
	if (AX_ERROR(status)){
		free(buffer);
		return status;
	}

	root->location = buffer;
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
		*(HKEY*)root->location,
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

