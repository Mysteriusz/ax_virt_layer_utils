/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_utility.h

	Abstract:
		This module defines default AX Project`s types, constants and functions 
		that provide an interface for programming and interacting with AX environment.

*/

#if defined(AX_WINDOWS)
#include "windows.h"
#else 
#error "No target operating system defined for the build"
#endif

#include "stdint.h"

#if !defined(AX_UTILITY)
#define AX_UTILITY

#define AX_OUT
#define AX_IN
#define AX_IN_OUT

#define AX_OUT_OPT
#define AX_IN_OPT
#define AX_IN_OUT_OPT

// Error check
#define AX_ERROR	(code)		((code != AX_SUCCESS))
typedef uint32_t AXSTATUS;

#define AX_SUCCESS 			0x00000000
#define AX_INVALID_COMMAND	 	0x00000001
#define AX_MEMORY_ERROR 		0x00000002
#define AX_INVALID_ARGUMENT		0x00000003

#define AX_DRIVER_NAME 			L"ax_virt_layer"
#define AX_CONTROL_NAME  		L"ax_virt_control" 

#define AX_CACHE_SIZE			0x200
#define AX_UPDATE_PATH 			L"\\update"

#if defined(AX_WINDOWS)

// Flag indicating if value is Windows LRESULT
#define AX_STATUS_LRESULT 		0x80000000
// Flag indicating if value is Windows GetLastError()
#define AX_STATUS_LERROR 		0x40000000

// Native error check
#define AX_NERROR       (code)		((code & AX_STATUS_LRESULT) || (code & AX_STATUS_LERROR))

// Windows names for driver and controller
#define AX_DRIVER_FULLNAME 		L"\\" AX_DRIVER_NAME L".sys"
#define AX_CONTROL_FULLNAME 		L"\\" AX_CONTROL_NAME L".exe"

// AX Virtualization service
#define AX_SERVICE_NAME 		L"AX_VIRTUALIZATION_DRIVER"
#define AX_SERVICE_DISPLAY_NAME 	L"AX_VIRTUALIZATION_PLATFORM"
#define AX_SERVICE_ACCESS 		GENERIC_ALL 
#define AX_SERVICE_TYPE 		SERVICE_KERNEL_DRIVER
#define AX_SERVICE_START 		SERVICE_DEMAND_START

// AX Control service
#define AX_CSERVICE_NAME 		L"AX_VIRTUALIZATION_CONTROL"
#define AX_CSERVICE_DISPLAY_NAME 	L"AX_VIRTUALIZATION_PLATORM_CONTROL"
#define AX_CSERVICE_ACCESS 		GENERIC_ALL 
#define AX_CSERVICE_TYPE 		SERVICE_WIN32_OWN_PROCESS
#define AX_CSERVICE_START 		SERVICE_AUTO_START

// Data location on Windows is in the registry
#define AX_DATA_ROOT_HKEY 		HKEY_LOCAL_MACHINE
#define AX_DATA_ROOT_SUBKEY 		L"SOFTWARE"
// Data is located in the registry at: 
//
// HKEY_LOCAL_MACHINE\\SOFTWARE\\AX_VIRTUALIZATION
//

#define AX_DATA_ROOT_NAME 		L"AX_VIRTUALIZATION"

#endif

typedef enum {
	DIRECTORY,

#if defined(AX_WINDOWS)
	REGISTRY,
#endif

} AX_DATA_LOCATION_TYPE;

/*
	Root of the configuration data.

	IMPORTANT NOTICE:

	If type is equal to DIRECTORY location is CHAR*
	If type is equal to REGISTRY location is HKEY* 
*/
typedef struct {
	void 			*location;
	size_t 			location_size;
	AX_DATA_LOCATION_TYPE 	type;	
} AX_DATA_ROOT;

AXSTATUS ax_get_data_root(
	AX_IN_OUT AX_DATA_ROOT 		*root	
);

typedef struct {
	wchar_t 		*name;
	void 			*value;
	size_t 			value_size;

#if defined(AX_WINDOWS)
	uint32_t 		reg_type; // Windows registry value type	
#endif

} AX_DATA_NODE; 

#define AX_DATA_NODE_COUNT_D 		0x04
#define AX_DATA_NODE_SIZE_D		AX_CACHE_SIZE * 2 
AX_DATA_NODE *ax_get_data_node_d(
	void
);

#if defined(AX_WINDOWS)

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

#endif // AX_WINDOWS

AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT	*root,
	AX_IN_OUT AX_DATA_NODE		*node
);
AXSTATUS ax_set_data(
	AX_IN const AX_DATA_ROOT	*root,
	AX_IN const AX_DATA_NODE	*node
);
void ax_free_data(
	AX_IN AX_DATA_NODE 		*node
);

#if defined(_MSC_VER)
	#define AX_NORETURN __declspec(noreturn)	
#elif defined(__GNUC__) || (__clang__) 
	#define AX_NORETURN __attribute__(noreturn)	
#endif

AX_NORETURN static void AX_CRASH(
	void
){
	abort();
}

#endif // AX_UTIL

