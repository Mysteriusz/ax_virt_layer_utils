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
	AX_IN wchar_t*			node_label,
	AX_IN_OUT AX_DATA_NODE*		node
){
	if (root == NULL
	|| node == NULL){
		return AX_INVALID_ARGUMENT;
	}

	if (root->type != DATA_TYPE_FILE
	|| root->location == NULL
	|| node->name == NULL
	|| node_label == NULL){
		return AX_INVALID_DATA;
	}

	AXSTATUS status = AX_SUCCESS;
	LRESULT result = NO_ERROR;

	wchar_t* file_buffer = NULL;
	size_t file_buffer_size = 0;

	file_buffer_size = GetFileSize(*(HANDLE*)root->location, NULL);
	file_buffer = malloc(file_buffer_size);

	DWORD read = 0;

	SetFilePointer(*(HANDLE*)root->location, 0, NULL, FILE_BEGIN);

	bool file_read_success = ReadFile(
		*(HANDLE*)root->location,
		file_buffer,
		(DWORD)file_buffer_size,
		&read,
		NULL
	);

	result = GetLastError();
	if (file_read_success != true 
	|| result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	//wchar_t* label_start = NULL; 
	//status = ax_find_text(file_buffer, node_label, &label_start); 
	if (AX_ERROR(status)){
		free(file_buffer);
		return status;
	}

	/*struct AX_READER_SETTINGS settings = (struct AX_READER_SETTINGS){
		.label = node_label,
		.char_set = AX_DEFAULT_CHAR_SET,
	};*/
	/*status = ax_read_range(
		label_start,
		&settings,
		&(wchar_t*)node->value,
		&node->value_size
	);*/

	if (AX_ERROR(status)){
		free(file_buffer);
		return status;
	}
	
	struct AX_DATA_FILE_INFO* context_buffer = malloc(sizeof(struct AX_DATA_FILE_INFO));
	context_buffer->label = _wcsdup(node_label);
	context_buffer->path = _ax_get_file_path(*(HANDLE*)root->location);

	node->context = context_buffer;

	free(file_buffer);

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

	LRESULT result = NO_ERROR;

	wchar_t* label_buffer = ((struct AX_DATA_FILE_INFO*)node->context)->label;
	bool file_write_success = false; 

	file_write_success = WriteFile(
		*(HANDLE*)root->location,
		label_buffer,
		(DWORD)_ax_size_wc(label_buffer) - sizeof(wchar_t),
		NULL,
		NULL
	);

	if ((file_write_success != true
	|| result != NO_ERROR)
	&& label_buffer != NULL){
		return result | AX_STATUS_LERROR;
	}

	file_write_success = WriteFile(
		*(HANDLE*)root->location,
		node->value,
		(DWORD)node->value_size - sizeof(wchar_t),
		NULL,
		NULL
	);

	if (file_write_success != true
	|| result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	file_write_success = WriteFile(
		*(HANDLE*)root->location,
		L"\n",
		(DWORD)sizeof(wchar_t),
		NULL,
		NULL
	);

	if (file_write_success != true
	|| result != NO_ERROR){
		return result | AX_STATUS_LERROR;
	}

	return AX_SUCCESS;
}
