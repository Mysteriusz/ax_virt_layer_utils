#include "ax_utility.h"

AXSTATUS ax_get_data_root(
	AX_OUT AX_DATA_ROOT* root	
){
	if (root == NULL) return AX_INVALID_ARGUMENT;

#if defined (AX_WINDOWS)
	HKEY buffer;
	LRESULT result = RegOpenKeyExA(AX_DATA_ROOT_HKEY, AX_DATA_ROOT_PATH, 0, KEY_ALL_ACCESS, &buffer);

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	root->location = &buffer;
	root->locationSize = sizeof(HKEY);
	root->type = REGISTRY;
#endif
	return AX_SUCCESS; 
}
 
AXSTATUS ax_get_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN_OUT AX_DATA_NODE* node
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

#if defined(AX_WINDOWS)

#endif

	return AX_SUCCESS; 
}
AXSTATUS ax_set_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN AX_DATA_NODE* node
){
	return AX_SUCCESS; 
}

