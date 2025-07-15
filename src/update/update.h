#include "windows.h"

#ifndef AX_UPDATE_UTIL
#define AX_UPDATE_UTIL

#define UPDATE_SUCCESS 			0x00000000
#define UPDATE_INVALID_COMMAND	 	0x00000001
#define UPDATE_MEMORY_ERROR 		0x00000002
#define UPDATE_INVALID_ARGUMENT		0x00000003
#define UPDATE_ERROR(code)(code != UPDATE_SUCCESS)
typedef UINT32 UPDATE_STATUS;

#define UPDATE_REG_MAIN_KEY L"AX_VIRTUALIZATION"
typedef struct _UPDATE_REG_KEY{
   PWCHAR name;
   DWORD type;
} UPDATE_REG_KEY, *PUPDATE_REG_KEY;

#define UPDATE_REG_BSKEY 0
#define UPDATE_REG_UPKEY 1
#define UPDATE_REG_DVKEY 2
static UPDATE_REG_KEY UPDATE_REG_KEYS[] = {
    {L"base_directory", REG_SZ},
    {L"update_directory", REG_SZ},
    {L"driver_path", REG_SZ},
};

UPDATE_STATUS Update_Get_Registry(
    PHKEY reg 
);    
UPDATE_STATUS Update_Set_Registry(
    PUPDATE_REG_KEY key,
    PVOID buffer,
    DWORD buffer_size
);

#endif

