#if defined(AX_WINDOWS)
#include "windows.h"
#else 
#error "No target operating system defined for the build"
#endif

#ifndef AX_UTILITY
#define AX_UTILITY

#define AX_OUT
#define AX_IN
#define AX_IN_OUT

#define AX_SUCCESS 			0x00000000
#define AX_INVALID_COMMAND	 	0x00000001
#define AX_MEMORY_ERROR 		0x00000002
#define AX_INVALID_ARGUMENT		0x00000003
#define AX_ERROR(code)			(code != AX_SUCCESS)
typedef unsigned int AXSTATUS;

typedef enum {
	DIRECTORY,
	// REGISTRY type is windows specific
	REGISTRY
} AX_DATA_LOCATION_TYPE;

/*
	Root of the configuration data.

	IMPORTANT NOTICE:

	If type is equal to DIRECTORY location is CHAR*
	If type is equal to REGISTRY location is HKEY* 
*/
typedef struct {
	void* location;
	unsigned int locationSize;
	AX_DATA_LOCATION_TYPE type;	
} AX_DATA_ROOT;

AXSTATUS ax_get_data_root(
	AX_OUT AX_DATA_ROOT* root	
);

typedef struct {
	char* name;
	void* value;
	unsigned int valueSize;
#if defined(AX_WINDOWS)
	unsigned char regType;	
#endif
} AX_DATA_NODE;
AXSTATUS ax_get_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN_OUT AX_DATA_NODE* node
);
AXSTATUS ax_set_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN AX_DATA_NODE* node
);

#endif

