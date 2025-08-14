#include "ax_utility.h"

static AXSTATUS _ax_open_data_root_dir(
	AX_IN_OUT AX_DATA_ROOT*		root,
	AX_IN_OPT wchar_t* 		path
){
	if (root == NULL){
		return AX_INVALID_ARGUMENT;
	}

	// Set the buffer to default if provided path is NULL
	wchar_t* path_buffer = path != NULL 
		? path 
		: AX_DEFAULT_DATA_ROOT_DIRECTORY;
	size_t path_buffer_size = 0;

	// Write expanded to the path buffer
	path_buffer = _ax_expand_path(path_buffer);
	path_buffer_size = _ax_size_w(path_buffer);

	uint32_t attributes = GetFileAttributesW(path_buffer);

	// Check if path exists and is a directory and not a file 
	if (attributes != INVALID_FILE_ATTRIBUTES 
		&& attributes & FILE_ATTRIBUTE_DIRECTORY){
		free((void*)path_buffer);
		return AX_INVALID_DATA;
	}
	
	// Write back to the provided root
	root->location = (void*)path_buffer;
	root->location_size = path_buffer_size;
	root->type = DATA_TYPE_DIRECTORY;

	return AX_SUCCESS;
}
static AXSTATUS _ax_get_data_dir(
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
static AXSTATUS _ax_set_data_dir(
	AX_IN AX_DATA_ROOT*		root,
	AX_IN AX_DATA_NODE*		node
){
	if (root == NULL
		|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_DIRECTORY
		|| node->value == NULL){
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

