#if defined(AX_WINDOWS)
#include "windows.h"
#else 
#error "No target operating system defined for the build"
#endif

#if !defined(AX_UTILITY)
#define AX_UTILITY

#define AX_OUT
#define AX_IN
#define AX_IN_OUT

#define AX_SUCCESS 			0x00000000
#define AX_INVALID_COMMAND	 	0x00000001
#define AX_MEMORY_ERROR 		0x00000002
#define AX_INVALID_ARGUMENT		0x00000003
#define AX_STATUS_LRESULT 		0x80000000

// Native error check
#define AX_NERROR       (code)		(code & AX_STATUS_LRESULT)
// Error check
#define AX_ERROR	(code)		(code != AX_SUCCESS)
typedef unsigned int AXSTATUS;

#if defined(AX_WINDOWS)
// Data location on WINDOWS is in the registry
#define AX_DATA_ROOT_HKEY HKEY_LOCAL_MACHINE
#define AX_DATA_ROOT_PATH "SOFTWARE\\AX_VIRTUALIZATION"
#endif

typedef enum {
	DIRECTORY,
	// REGISTRY type is Windows specific
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

#if defined(AX_WINDOWS)
// Base directory Windows registry key
#define AX_DATA_NODE_BSD { \
	"base_directory", \
	NULL, \
	0, \
	REG_SZ, \
}
// Driver path Windows registry key
#define AX_DATA_NODE_DVP { \
	"driver_path", \
	NULL, \
	0, \
	REG_SZ, \
}
// Update directory Windows registry key
#define AX_DATA_NODE_UPD { \
	"update_directory", \
	NULL, \
	0, \
	REG_SZ, \
}
#endif

#define AX_DEFAULT_DATA_NODE_COUNT 3
static AX_DATA_NODE ax_default_data_nodes[AX_DEFAULT_DATA_NODE_COUNT] = {
	AX_DATA_NODE_BSD,
	AX_DATA_NODE_DVP,
	AX_DATA_NODE_UPD
};

AXSTATUS ax_get_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN_OUT AX_DATA_NODE* node
);
AXSTATUS ax_set_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN AX_DATA_NODE* node,
	AX_IN void* buffer,
	AX_IN unsigned int bufferSize
);
void ax_free_data(
	AX_IN AX_DATA_NODE* node
);

#endif

