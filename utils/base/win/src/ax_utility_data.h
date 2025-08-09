#if !defined(AX_UTILITY_DATA_INT)
#define AX_UTILITY_DATA_INT

#include "ax_utility.h"
#include "ax_control.h"
#include "ax_driver.h"

// Data location on Windows is in the registry
#define AX_DATA_ROOT_HKEY 		HKEY_LOCAL_MACHINE
#define AX_DATA_ROOT_SUBKEY 		L"SOFTWARE"
// Data is located in the registry at: 
//
// HKEY_LOCAL_MACHINE\\SOFTWARE\\AX_VIRTUALIZATION
//

#define AX_DATA_ROOT_NAME 		L"AX_VIRTUALIZATION"

typedef uint8_t AX_DATA_TYPE;
enum {
	DIRECTORY = 0, // context == wchar_t* 
	REGISTRY = 1, // context == uint32_t* (Windows registry value key) 
	// FUTURE
	SERVER = 2, 
	SECURE_SERVER = 3
};

/*
	Root of the configuration data.

	IMPORTANT NOTICE:

	If type is equal to DIRECTORY location is wchar_t*
	If type is equal to REGISTRY location is void* (HKEY) 
*/
typedef struct {
	void* 			location;
	size_t 			location_size;
	AX_DATA_TYPE 		type;	
} AX_DATA_ROOT;

#define AX_DEFAULT_DATA_ROOT_TYPE 	(AX_DATA_TYPE)REGISTRY
AXSTATUS ax_open_data_root(
	AX_OUT const AX_DATA_ROOT* 	root,	
	AX_IN_OPT const AX_DATA_TYPE* 	type
);

typedef struct {
	wchar_t* 		name;
	void* 			value;
	size_t 			value_size;
	AX_DATA_TYPE   		type;
	void*			context;
} AX_DATA_NODE; 

static wchar_t* ax_load_working_directory(
	void
){
	uint32_t buffer_size = 0;
	buffer_size = GetCurrentDirectoryW(0, NULL);
	wchar_t* buffer = malloc(buffer_size);
	GetCurrentDirectoryW(buffer_size, buffer);

	// Assert current directory gather not failing
	assert(GetLastError() == NO_ERROR);

	return buffer;
}

#define AX_DEFAULT_NODE_COUNT 		0x0004
#define AX_DATA_NODE_BSD(node_type) \
	(AX_DATA_NODE){ \
		.name = L"base_directory", \
		.value = NULL, \
		.value_size = 0, \
		.type = node_type, \
		.context = NULL, \
	} 
#define AX_DATA_NODE_UPD(node_type) \
	(AX_DATA_NODE){ \
		.name = L"update_directory", \
		.value = NULL, \
		.value_size = 0, \
		.type = node_type, \
		.context = NULL, \
	} 
#define AX_DATA_NODE_DVP(node_type) \
	(AX_DATA_NODE){ \
		.name = L"driver_path", \
		.value = NULL, \
		.value_size = 0, \
		.type = node_type, \
		.context = NULL, \
	} 
#define AX_DATA_NODE_CTP(node_type) \
	(AX_DATA_NODE){ \
		.name = L"control_path", \
		.value = NULL, \
		.value_size = 0, \
		.type = node_type, \
		.context = NULL, \
	} 

AXSTATUS ax_get_default_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_OUT AX_DATA_NODE**		node_array,
	AX_OUT uint32_t*		node_count
);
AXSTATUS ax_set_default_data(
	AX_IN const AX_DATA_ROOT*	root
);

AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN_OUT AX_DATA_NODE*		node
);
AXSTATUS ax_set_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN const AX_DATA_NODE*	node
);

void ax_free_root(
	AX_IN_OUT AX_DATA_ROOT*		root
);
void ax_free_data(
	AX_IN AX_DATA_NODE* 		node
);

#endif // !defined(AX_UTILITY_DATA_INT)

