#include "ax_utility.h"

AX_DATA_NODE* ax_get_default_data_nodes(
	void
){
	static AX_DATA_NODE ax_default_data_nodes[AX_DEFAULT_DATA_NODE_COUNT] = {0};
	static int init = 0;

	if (init != 1){
		ax_default_data_nodes[0] = (AX_DATA_NODE)AX_DATA_NODE_BSD;
		ax_default_data_nodes[1] = (AX_DATA_NODE)AX_DATA_NODE_DVP;
		ax_default_data_nodes[2] = (AX_DATA_NODE)AX_DATA_NODE_UPD;
	}
	init = 1;

	return ax_default_data_nodes;
}

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
	AX_IN AX_DATA_NODE* node
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

#if defined(AX_WINDOWS)
	LRESULT result = 0;

	result = RegSetValueExA(root->location, node->name, 0, node->regType, node->value, node->valueSize); if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}
#endif

	return AX_SUCCESS; 
}

void ax_free_data(
	AX_IN AX_DATA_NODE* node
){
	free(node->value);
}

