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

