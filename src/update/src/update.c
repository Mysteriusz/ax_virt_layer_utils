#include "../setup.h"

int main(){
    int arg_count = 0;
    LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &arg_count);

    UPDATE_SETUP_DATA data;
    UPDATE_STATUS pstatus = UPDATE_SUCCESS;

    for (int i = 0; i < arg_count; i++){
	if (arg_list[i] != NULL && arg_list[i][0] == '-'){
	    PWCHAR arg = arg_list[i];
	    i++;
	    PWCHAR val = arg_list[i];

	    size_t alloc_size = (wcslen(val) + 1) * sizeof(wchar_t);
	    
	    PWCHAR* buffer = NULL;
	    if (wcscmp(arg, UPDATE_SETUP_DIRECTORY_ARG) == 0){
		buffer = &data.setup_directory;
	    }
	    else if (wcscmp(arg, UPDATE_SETUP_DRIVER_ARG) == 0){
		buffer = &data.driver_path;
	    }
	    else{
		printf("%ls", arg);
		pstatus = UPDATE_INVALID_ARGUMENT;
		break;
	    }
    
	    *buffer = malloc(alloc_size);    
	
	    if (*buffer == NULL){
		pstatus = UPDATE_MEMORY_ERROR;
		break;
	    }
    
	    memcpy(*buffer, val, alloc_size);
	}
    }

    if (UPDATE_ERROR(pstatus)){
	return pstatus;
    }
    Update_Setup(&data);

    if (data.setup_directory) free(data.setup_directory);
    if (data.driver_path) free(data.driver_path);
    
    LocalFree(arg_list);
    return 0;

}
