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

#define AX_DRIVER_FULLNAME "\\ax_virt_layer.sys"
#define AX_DRIVER_BASENAME "ax_virt_layer"
#define AX_UPDATE_PATH "\\update"

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

#define AX_DEFAULT_DATA_NODE_COUNT 3
AX_DATA_NODE* ax_get_default_data_nodes(
	void
);

#if defined(AX_WINDOWS)
/*
	Methods below are default directory getters for easy modifications 
	They are to be used with extreme caution given they allocate memory (_strdup)
*/
static char* get_cached_dir(){
	static char dir[MAX_PATH];
	memset(dir, 0, MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH, dir);	
	return _strdup(dir);
}
static char* cat_cached_dir(char* a, char* b){
	static char dir[MAX_PATH];
	memset(dir, 0, MAX_PATH);
	strcpy_s(dir, MAX_PATH, a); 
	strcat_s(dir, MAX_PATH, b); 
	return _strdup(dir);
}

// Base directory Windows registry key
#define AX_DATA_NODE_BSD_V get_cached_dir()
#define AX_DATA_NODE_BSD { \
	"base_directory", \
	AX_DATA_NODE_BSD_V, \
	MAX_PATH, \
	REG_SZ, \
}

// Driver path Windows registry key
#define AX_DATA_NODE_DVP_V cat_cached_dir(get_cached_dir(), AX_DRIVER_FULLNAME)
#define AX_DATA_NODE_DVP { \
	"driver_path", \
	AX_DATA_NODE_DVP_V, \
	MAX_PATH, \
	REG_SZ, \
}

// Update directory Windows registry key
#define AX_DATA_NODE_UPD_V cat_cached_dir(get_cached_dir(), AX_UPDATE_PATH)
#define AX_DATA_NODE_UPD { \
	"update_directory", \
	AX_DATA_NODE_UPD_V, \
	MAX_PATH, \
	REG_SZ, \
}
#endif

AXSTATUS ax_get_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN_OUT AX_DATA_NODE* node
);
AXSTATUS ax_set_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN AX_DATA_NODE* node
);
void ax_free_data(
	AX_IN AX_DATA_NODE* node);

#endif

