#include "update.h"

#ifndef AX_UPDATE_UTIL
#error "UPDATE UTILITY WAS NOT DEFINED!"
#endif

#ifndef AX_UPDATE_SETUP_UTIL
#define AX_UPDATE_SETUP_UTIL

#define UPDATE_SETUP_DIRECTORY_ARG L"-setup"
#define UPDATE_SETUP_DRIVER_ARG L"-driver"

typedef struct _UPDATE_SETUP_DATA{
    PWCHAR setup_directory;
    PWCHAR driver_path;
} UPDATE_SETUP_DATA, *PUPDATE_SETUP_DATA;

UPDATE_STATUS Update_Setup_Registry(
    PUPDATE_SETUP_DATA data
);

#endif

