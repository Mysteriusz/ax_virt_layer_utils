#include "../update.h"

UPDATE_STATUS Update_Get_Registry(
    PHKEY reg 
){
    if (reg == NULL) return UPDATE_INVALID_ARGUMENT;

    HKEY root_reg;
    RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE", 0, KEY_CREATE_SUB_KEY, &root_reg); 
    RegOpenKeyExW(root_reg, UPDATE_REG_MAIN_KEY, 0, KEY_SET_VALUE, reg);

    return UPDATE_SUCCESS;
}
UPDATE_STATUS Update_Set_Registry(
    PUPDATE_REG_KEY key_info,
    PVOID buffer,
    DWORD buffer_size
){
    if (key_info == NULL || buffer == NULL || buffer_size == 0) return UPDATE_INVALID_ARGUMENT;
    
    HKEY root_reg;
    Update_Get_Registry(&root_reg);

    RegSetValueExW(root_reg, key_info->name, 0, key_info->type, buffer, buffer_size);

    return UPDATE_SUCCESS;
}

