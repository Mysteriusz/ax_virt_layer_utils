/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_utility.h

	Abstract:
		This module defines default WINDOWS AX Project`s types, constants and functions 
		that provide an interface for programming and interacting with AX environment.

*/

#if !defined(AX_UTILITY_INT)
#define AX_UTILITY_INT

#if defined(AX_UM) 
#include "windows.h"
#elif defined(AX_KM)
#include "ntddk.h"
#endif

#include "stdint.h"
#include "stdio.h"

#if !defined(AX_WINDOWS)
#define AX_WINDOWS
#endif

#define AX_OUT
#define AX_IN
#define AX_IN_OUT

#define AX_OUT_OPT
#define AX_IN_OPT
#define AX_IN_OUT_OPT

#define AX_CACHE_SIZE			0x200
#define AX_UPDATE_PATH 			L"\\update"

#include "ax_error.h"

/*
	
	Default driver configuration interface.

*/

// Data location on Windows is in the registry
#define AX_DATA_ROOT_HKEY 		HKEY_LOCAL_MACHINE
#define AX_DATA_ROOT_SUBKEY 		L"SOFTWARE"
// Data is located in the registry at: 
//
// HKEY_LOCAL_MACHINE\\SOFTWARE\\AX_VIRTUALIZATION
//

#define AX_DATA_ROOT_NAME 		L"AX_VIRTUALIZATION"

typedef uint8_t AX_DATA_LOCATION_TYPE;
enum {
	DIRECTORY = 0,
	REGISTRY = 1,
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
	AX_DATA_LOCATION_TYPE 	type;	
} AX_DATA_ROOT;

AXSTATUS ax_get_data_root(
	AX_IN_OUT AX_DATA_ROOT* 	root	
);

typedef struct {
	wchar_t* 		name;
	void* 			value;
	size_t 			value_size;
	uint32_t 		reg_type; // Windows registry value type	
} AX_DATA_NODE; 

#define AX_DATA_NODE_COUNT_D 		0x04
#define AX_DATA_NODE_SIZE_D		AX_CACHE_SIZE * 2 
AX_DATA_NODE *ax_get_data_node_d(
	void
);

// Base directory Windows registry key 
#define AX_DATA_NODE_BSD(v)					\
	(AX_DATA_NODE){ 					\
		.name=			L"base_directory",	\
		.value=			(wchar_t*)v, 		\
		.value_size= 		AX_DATA_NODE_SIZE_D, 	\
		.reg_type=		REG_SZ, 		\
	}

// Driver path Windows registry key 
#define AX_DATA_NODE_DVP(v)					\
	(AX_DATA_NODE){ 					\
		.name=			L"driver_path",		\
		.value=			(wchar_t*)v, 		\
		.value_size= 		AX_DATA_NODE_SIZE_D, 	\
		.reg_type=		REG_SZ, 		\
	}

// Controller path Windows registry key 
#define AX_DATA_NODE_CTP(v)					\
	(AX_DATA_NODE){ 					\
		.name=			L"controller_path",	\
		.value=			(wchar_t*)v, 		\
		.value_size= 		AX_DATA_NODE_SIZE_D, 	\
		.reg_type=		REG_SZ, 		\
	}

// Update directory Windows registry key
#define AX_DATA_NODE_UPD(v)					\
	(AX_DATA_NODE){ 					\
		.name=			L"update_directory",	\
		.value=			(wchar_t*)v, 		\
		.value_size= 		AX_DATA_NODE_SIZE_D, 	\
		.reg_type=		REG_SZ, 		\
	}

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

#endif // !defined(AX_UTILITY_INT)

