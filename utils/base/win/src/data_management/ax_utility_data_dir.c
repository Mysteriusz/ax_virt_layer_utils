#include "ax_utility.h"

static AXSTATUS ax_open_data_root_dir(
	AX_IN_OUT AX_DATA_ROOT*		root,
	AX_IN_OPT wchar_t* 		path
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	// Set the buffer to default if provided path is NULL
	wchar_t* path_buffer = 
		path != NULL 
		? path 
		: AX_DEFAULT_DATA_ROOT_DIRECTORY;
	size_t path_buffer_size = (wcslen(path_buffer) + 1) * sizeof(wchar_t);

	//
	// Expand environment variables if used in the path
	// ex: %USERPROFILE% -> C:\users\username
	//
	size_t temp_size = ExpandEnvironmentStringsW(path_buffer, NULL, 0) * sizeof(wchar_t);
	wchar_t* temp = malloc(temp_size);
	ExpandEnvironmentStringsW(path_buffer, temp, temp_size);

	// Write expanded back to the path buffer
	path_buffer = temp;
	path_buffer_size = temp_size;

	// Check if directory exists by getting its attributes
	bool directory_exists = 
		GetFileAttributesW(path_buffer) != INVALID_FILE_ATTRIBUTES 
		? true 
		: false;

	if (directory_exists == false){
		free((void*)path_buffer);
		return AX_INVALID_DATA;
	}
	
	// Write back to the provided root
	root->location = (void*)path_buffer;
	root->location_size = path_buffer_size;
	root->type = DATA_TYPE_DIRECTORY;

	return AX_SUCCESS;
}
static AXSTATUS ax_get_data_dir(
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

	wchar_t* buffer = NULL;
	wchar_t* buffer_context = NULL;
	size_t buffer_size = 0;

	uint32_t result = NO_ERROR;

	PathAllocCombine(
		root->location,
		node->name,
		PATHCCH_ALLOW_LONG_PATHS,
		&buffer_context
	);

	HANDLE file = CreateFileW(
		buffer_context,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	result = GetLastError();
	if (result != ERROR_ALREADY_EXISTS 
		&& result != NO_ERROR){
		free(buffer_context);
		return result | AX_STATUS_LERROR;
	}

	buffer_size = GetFileSize(file, NULL);
	buffer = malloc(buffer_size);

	bool file_read_success = ReadFile(
		file,
		buffer,
		buffer_size,
		NULL,
		NULL
	);

	result = GetLastError();
	if (!file_read_success 
		|| result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	node->value = buffer;
	node->value_size = buffer_size;
	node->context = buffer_context;

	return AX_SUCCESS;
}
static AXSTATUS ax_set_data_dir(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN AX_DATA_NODE*		node
){
	if (root == NULL
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_DIRECTORY){
		return AX_INVALID_DATA;
	}

	if (node->context == NULL){
		return AX_UNKNOWN_CONTEXT;
	}

	uint32_t result = NO_ERROR;

	HANDLE file = CreateFileW(
		(wchar_t*)node->context,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (result != ERROR_ALREADY_EXISTS 
		&& result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	bool file_write_success = WriteFile(
		file,
		node->value,
		node->value_size,
		NULL,
		NULL
	);

	if (!file_write_success
		|| result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	return AX_SUCCESS;
}

