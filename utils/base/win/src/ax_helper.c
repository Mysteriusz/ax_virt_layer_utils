#include "ax_helper.h"

wchar_t* _ax_load_working_directory(
	void
){
	wchar_t* buffer = NULL;
	size_t buffer_size = 0;

	buffer_size = GetCurrentDirectoryW(0, NULL);
	buffer = malloc(buffer_size);
	GetCurrentDirectoryW(buffer_size, buffer);

	return buffer;
}


/*

 	Allocate and expand environment variables if used in the path.

	Example: 
		%USERPROFILE% -> C:\users\username

*/
wchar_t* _ax_expand_path(
	AX_IN wchar_t*  		path
){
	if (path == NULL){
		return NULL;
	}

	wchar_t* buffer = NULL;
	size_t buffer_size = 0; 

	buffer_size = ExpandEnvironmentStringsW(path, NULL, 0) * sizeof(wchar_t);
	buffer = malloc(buffer_size);
	ExpandEnvironmentStringsW(path, buffer, buffer_size);

	// Assert function not failing. 
	assert(GetLastError() == NO_ERROR);

	return buffer;
}

wchar_t* _ax_get_file_path(
	AX_IN HANDLE 			file
){
	if (file == NULL){
		return NULL;
	}

	wchar_t* buffer = NULL;
	size_t buffer_size = 0; 

	buffer_size = GetFinalPathNameByHandleW(file, NULL, 0, FILE_NAME_OPENED) * sizeof(wchar_t);
	buffer = malloc(buffer_size);
	GetFinalPathNameByHandleW(file, buffer, buffer_size, FILE_NAME_OPENED);

	// Assert function not failing. 
	assert(GetLastError() == NO_ERROR);

	return buffer;
}

