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

	root->location = buffer;
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
	char* buffer = NULL;
	unsigned int bufferSize = 0;

	LRESULT result = 0;

	if (node->name == NULL 
	|| root->location == NULL) return AX_INVALID_ARGUMENT;

	result = RegQueryValueExA(root->location, node->name, NULL, NULL, buffer, &bufferSize); 		
	
	buffer = malloc(bufferSize);

	result = RegQueryValueExA(root->location, node->name, NULL, NULL, buffer, &bufferSize); 			
	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	node->value = buffer;
	node->valueSize = bufferSize;
#endif

	return AX_SUCCESS; 
}
AXSTATUS ax_set_data(
	AX_IN AX_DATA_ROOT* root,
	AX_IN AX_DATA_NODE* node,
	AX_IN void* buffer,
	AX_IN unsigned int bufferSize
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

	LRESULT result = 0;

	result = RegSetValueExA(root->location, node->name, 0, node->regType, buffer, bufferSize); 

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	return AX_SUCCESS; 
}

void ax_free_data(
	AX_IN AX_DATA_NODE* node
){
	free(node->value);
}

