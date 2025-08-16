#if !defined(AX_UTILITY_DATA_INT)
#define AX_UTILITY_DATA_INT

#include "ax_utility.h"

// Data location on Windows is in the registry
#define AX_DEFAULT_SYSTEM_ROOT_REG 	HKEY_LOCAL_MACHINE
#define AX_DEFAULT_DATA_ROOT_REG 	L"SOFTWARE\\AX_VIRTUALIZATION"
/* 
	Data is located in the registry at: 

 		- HKEY_LOCAL_MACHINE\\SOFTWARE\\AX_VIRTUALIZATION

*/

// Default DATA_TYPE_DIRECTORY root->location
#define AX_DEFAULT_DATA_ROOT_DIRECTORY 	L"%USERPROFILE%\\Documents"
// Default DATA_TYPE_DIRECTORY root->location
#define AX_DEFAULT_DATA_ROOT_FILE 	L"%USERPROFILE%\\Documents\\ax_data.cfg"

typedef uint8_t AX_DATA_TYPE;
enum _AX_DATA_TYPE {
	DATA_TYPE_DIRECTORY = 0, // context == wchar_t* (Directory path ex: root->location + node->name) 
	DATA_TYPE_FILE = 1, // context ==  _AX_DATA_FILE_INFO  
	DATA_TYPE_REGISTRY = 2, // context == uint32_t* (Windows registry value key ex: REG_SZ) 
	// FUTURE
	DATA_TYPE_SERVER = 3, 
	DATA_TYPE_SECURE_SERVER = 4
};

/*

	Root of the configuration data.

	IMPORTANT NOTICE:

	If type is equal to DIRECTORY location is allocted wchar_t* (Directory path)
	If type is equal to FILE location is allocted int (HANDLE)
	If type is equal to REGISTRY location is allocated int (HKEY) 

*/
typedef struct _AX_DATA_ROOT {
	void* 			location;
	size_t 			location_size;
	AX_DATA_TYPE 		type;	
} AX_DATA_ROOT;

typedef struct _AX_DATA_NODE {
	wchar_t* 		name;
	void* 			value;
	size_t 			value_size;
	AX_DATA_TYPE   		type;
	void*			context;
} AX_DATA_NODE; 

typedef struct _AX_DATA_FILE_INFO {
	wchar_t* 		path; // Path to the file.
	wchar_t* 		label; // Ex: [base_directory]: (content)
} AX_DATA_FILE_INFO;

/*
 	
	Defaults

*/

#define AX_DEFAULT_DATA_ROOT_TYPE 	(AX_DATA_TYPE)DATA_TYPE_REGISTRY
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

/*

	Opens (allocates) a new root for the data.
	
	WORKFLOW:

	Function check (if provided) type or uses AX_DEFAULT_DATA_ROOT_TYPE macro, 
	and given that value calls handler function for the requested type.

	Write-back to the root occurs only when return value is false in the ax_error(axstatus) statement. 
	
	IMPORTANT NOTICE:

	For type EQUAL (DATA_TYPE_DIRECTORY) context is used as a path to the root location. 
	(Directory which should be used as root)
	Example: 
		context = L"C:\path\to\root\directory";

	For type EQUAL (DATA_TYPE_REGISTRY) context is NOT used. 

	write-back to the root occurs only when return value is false in the ax_error(axstatus) statement. 
	REMEMBER:

	After calling this function its necessary to call:

		- ax_free_root

	to safely free data and prevent memory leaks.

*/
AXSTATUS ax_open_data_root(
	AX_OUT const AX_DATA_ROOT* 	root,	
	AX_IN_OPT const AX_DATA_TYPE* 	type,
	AX_IN_OPT void*			context
);

/*
	Gets (allocates) array of DEFAULT nodes and assigns DEFAULT values.
	
	WORKFLOW:

	Function allocates memory and copies all default node data to be buffer.
	Then it fills all the nodes with runtime info depending on the provided root->type variable.

	Write-back to the node_array and node_count occurs ONLY
	when return value is false in the AX_ERROR(AXSTATUS) statement. 

	IMPORTANT NOTICE:

	For root->type EQUAL (DATA_TYPE_DIRECTORY) context is allocated and set 
	to FULLNAME of the file (node).
	Example:
		context = L"C:\path\to\root\directory\base_directory"; (For the node.name = L"base_directory";)

	For root->type EQUAL (DATA_TYPE_REGISTRY) context is allocated and set
	to WINDOWS REGISTRY VALUE TYPE required to store the key (node).
	Example:
		context = REG_SZ; (Null terminated string value type)

	REMEMBER:

	After calling this function its necessary to call:

		- ax_free_data_array

	to safely free data and prevent memory leaks.

*/
AXSTATUS ax_get_default_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_OUT AX_DATA_NODE**		node_array,
	AX_OUT uint32_t*		node_count
);

/*
	Sets (writes) array of DEFAULT nodes.
	
	WORKFLOW:

	Function calls 
		- ax_get_default_data 
	to gather list of default nodes with provided root.
	Then it loops all of them and calls 
		- ax_set_data 
	to write each node value.

	IMPORTANT NOTICE:

	If any node fails, the loop is IMMEDIATELY stopped and the error code is returned.

*/
AXSTATUS ax_set_default_data(
	AX_IN const AX_DATA_ROOT*	root
);

/*
	Gets (allocates) node.value and node.value_size based on node.name field.
	
	WORKFLOW:

	Function depending on the root->type calls internal function,
	for gathering info about the node.

	Write-back to the node occurs ONLY
	when return value is false in the AX_ERROR(AXSTATUS) statement. 

	IMPORTANT NOTICE:

	For root->type EQUAL (DATA_TYPE_DIRECTORY) context is allocated and set 
	to FULLNAME of the file (node).
	Example:
		context = L"root->location\base_directory"; (For the node.name = L"base_directory";)

	For root->type EQUAL (DATA_TYPE_REGISTRY) context is allocated and set
	to WINDOWS REGISTRY VALUE TYPE required to store the key (node).
	Example:
		context = REG_SZ; (Null terminated string value type)

	REMEMBER:

	After calling this function its necessary to call:

		- ax_free_data 

	to safely free data and prevent memory leaks.

*/
AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN_OUT AX_DATA_NODE*		node
);

/*
	Sets (writes) node to the root.
	
	WORKFLOW:

	Function depending on the root->type calls internal function,
	for writing node value.

	Write of the node value occurs ONLY
	when return value is false in the AX_ERROR(AXSTATUS) statement. 

	IMPORTANT NOTICE:

	For root->type EQUAL (DATA_TYPE_DIRECTORY) context MUST 
	point to the node file.
	Example:
		context = L"node->context\base_directory"; (For the node.name = L"base_directory";)

	For root->type EQUAL (DATA_TYPE_REGISTRY) context MUST 
	point to the WINDOWS REGISTRY VALUE TYPE required to store the key (node).
	Example:
		context = REG_SZ; (Null terminated string value type)

*/
AXSTATUS ax_set_data(
	AX_IN const AX_DATA_ROOT*	root,
	AX_IN const AX_DATA_NODE*	node
);

void ax_free_root(
	AX_IN AX_DATA_ROOT*		root
);
void ax_free_data(
	AX_IN AX_DATA_NODE* 		node
);
void ax_free_data_array(
	AX_IN AX_DATA_NODE*	 	node_array,
	AX_IN uint32_t			node_count
);

#endif // !defined(AX_UTILITY_DATA_INT)

