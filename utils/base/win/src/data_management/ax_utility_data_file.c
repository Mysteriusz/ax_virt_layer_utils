#include "ax_utility.h"

static AXSTATUS _ax_open_data_root_file(
	AX_IN_OUT AX_DATA_ROOT*		root,
	AX_IN_OPT wchar_t* 		path
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	// Set the buffer to default if provided path is NULL
	wchar_t* path_buffer = path != NULL 
		? path 
		: AX_DEFAULT_DATA_ROOT_FILE;

	// Write expanded to the path buffer
	path_buffer = _ax_expand_path(path_buffer);

	// Try to create the root on provided path if it doesnt exist 
	HANDLE file = CreateFileW(
		path_buffer,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);	
		
	status = GetLastError();

	if (file == INVALID_HANDLE_VALUE
		|| (status != ERROR_ALREADY_EXISTS 
		&& status != NO_ERROR)){
		return status | AX_STATUS_LERROR;
	}

	HANDLE* file_buffer = malloc(sizeof(HANDLE));
	*file_buffer = file;

	// Write back to the provided root
	root->location = (void*)file_buffer;
	root->location_size = sizeof(HANDLE);
	root->type = DATA_TYPE_FILE;

	return AX_SUCCESS;
}

static AXSTATUS _ax_get_data_file(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_DIRECTORY){
		return AX_INVALID_DATA;
	}

	return AX_SUCCESS;
}

static AXSTATUS _ax_set_data_file(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN AX_DATA_NODE*		node
){
	if (root == NULL
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_FILE
		|| node->value == NULL){
		return AX_INVALID_DATA;
	}

	if (node->context == NULL){
		return AX_UNKNOWN_CONTEXT;
	}

	uint32_t result = NO_ERROR;

	AX_DATA_FILE_INFO* file_info = (AX_DATA_FILE_INFO*)node->context;

	if (file_info->label != NULL){

		// Write label
		WriteFile(
			*(HANDLE*)root->location,
			file_info->label,	
			(DWORD)_ax_size_w(file_info->label),
			NULL,
			NULL
		);

		result = GetLastError();
	
		if (result != NO_ERROR){
			return result | AX_STATUS_LERROR;
		}
	}

	// Write value
	WriteFile(
		*(HANDLE*)root->location,
		node->value,	
		(DWORD)node->value_size,
		NULL,
		NULL
	);

	result = GetLastError();
	
	if (result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	return AX_SUCCESS;
}

