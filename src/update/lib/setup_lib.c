#include "../setup.h"
#include "assert.h"

UPDATE_STATUS Update_Setup(
    PUPDATE_SETUP_DATA data 
){
    assert(data->setup_directory != NULL && data->driver_path != NULL);
    Update_Setup_Registry(data);

    return 0;
}
    
UPDATE_STATUS Update_Setup_Registry(
    PUPDATE_SETUP_DATA data
){
    if (data == NULL) return UPDATE_INVALID_ARGUMENT;

    HKEY main_reg;
    Update_Get_Registry(&main_reg);
    for (int i = 0; i < ARRAYSIZE(UPDATE_REG_KEYS); i++){
	PVOID key_buffer = NULL;
	DWORD key_buffer_size = 0;
	
	switch(i){
	case UPDATE_REG_BSKEY:
	    key_buffer = data->setup_directory;
	    key_buffer_size = (wcslen(key_buffer) + 1) * sizeof(WCHAR);
	    break;
	case UPDATE_REG_UPKEY:
	    key_buffer = data->setup_directory;
	    key_buffer_size = (wcslen(key_buffer) + 1) * sizeof(WCHAR);
	    break;
	case UPDATE_REG_DVKEY:
	    key_buffer = data->driver_path;
	    key_buffer_size = (wcslen(key_buffer) + 1) * sizeof(WCHAR);
	    break;
	default:
	    break;
	}

	Update_Set_Registry(&UPDATE_REG_KEYS[i], key_buffer, key_buffer_size);
    }

    return UPDATE_SUCCESS;
}



