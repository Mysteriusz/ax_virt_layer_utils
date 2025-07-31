#include "ax_utility.h"

AXSTATUS ax_get_data_root(
	AX_IN_OUT AX_DATA_ROOT 		*root	
){
	if (root == NULL) return AX_INVALID_ARGUMENT;

#if defined (AX_WINDOWS)

	LRESULT result = 0;
	HKEY buffer;

	result = RegOpenKeyExW(AX_DATA_ROOT_HKEY, AX_DATA_ROOT_SUBKEY, 0, KEY_CREATE_SUB_KEY, &buffer);

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	result = RegCreateKeyExW(buffer, AX_DATA_ROOT_NAME, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &buffer, NULL);

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	root->location = buffer;
	root->location_size = sizeof(HKEY);
	root->type = REGISTRY;

#endif
	return AX_SUCCESS; 
}

AX_DATA_NODE *ax_get_data_node_d(
	void
){
	static wchar_t cached_dir[AX_CACHE_SIZE];
	memset(cached_dir, 0, AX_CACHE_SIZE);
	
#if defined(AX_WINDOWS)

	// Allocate local buffer for current directory and copy cached_dir.
	// This prevents STATUS_STACK_BUFFER_OVERRUN which appears to be returned when buffer is too big.

	uint32_t dcs = 0;
	wchar_t *dcb = NULL;

	dcs = GetCurrentDirectoryW(0, NULL);
	dcb = malloc(dcs);
	dcs = GetCurrentDirectoryW(dcs, dcb);

	wcscpy_s(cached_dir, AX_CACHE_SIZE, dcb);
	free(dcb);

#endif
	static wchar_t bsd[AX_DATA_NODE_SIZE_D];
	wcscpy_s(bsd, AX_DATA_NODE_SIZE_D, cached_dir);

	static wchar_t dvp[AX_DATA_NODE_SIZE_D];
	wcscpy_s(dvp, AX_DATA_NODE_SIZE_D, cached_dir);
	wcscat_s(dvp, AX_DATA_NODE_SIZE_D, AX_DRIVER_FULLNAME);

	static wchar_t ctp[AX_DATA_NODE_SIZE_D];
	wcscpy_s(ctp, AX_DATA_NODE_SIZE_D, cached_dir);
	wcscat_s(ctp, AX_DATA_NODE_SIZE_D, AX_CONTROL_FULLNAME);

	static wchar_t upd[AX_DATA_NODE_SIZE_D];
	wcscpy_s(upd, AX_DATA_NODE_SIZE_D, cached_dir);
	wcscat_s(upd, AX_DATA_NODE_SIZE_D, AX_UPDATE_PATH);

	static AX_DATA_NODE default_nodes[AX_DATA_NODE_COUNT_D];
	default_nodes[0] = AX_DATA_NODE_BSD(bsd);
	default_nodes[1] = AX_DATA_NODE_DVP(dvp);
	default_nodes[2] = AX_DATA_NODE_CTP(ctp);
	default_nodes[3] = AX_DATA_NODE_UPD(upd);

	return default_nodes;
}

AXSTATUS ax_get_data(
	AX_IN const AX_DATA_ROOT	*root,
	AX_IN_OUT AX_DATA_NODE		*node
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

#if defined(AX_WINDOWS)

	wchar_t* buffer = NULL;
	size_t buffer_size = 0;

	LRESULT result = 0;

	if (node->name == NULL 
	|| root->location == NULL) return AX_INVALID_ARGUMENT;

	result = RegQueryValueExW(root->location, node->name, NULL, NULL, (char*)buffer, (uint32_t*)&buffer_size); 		
	
	buffer = malloc(buffer_size);

	result = RegQueryValueExW(root->location, node->name, NULL, NULL, (char*)buffer, (uint32_t*)&buffer_size); 			

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

	node->value = buffer;
	node->value_size = buffer_size;

#endif

	return AX_SUCCESS; 
}
AXSTATUS ax_set_data(
	AX_IN const AX_DATA_ROOT	*root,
	AX_IN const AX_DATA_NODE	*node
){
	if (root == NULL 
	|| node == NULL) return AX_INVALID_ARGUMENT;

#if defined(AX_WINDOWS)

	LRESULT result = RegSetValueExW(root->location, node->name, 0, node->reg_type, node->value, node->value_size); 

	if (result != ERROR_SUCCESS){
		return result | AX_STATUS_LRESULT;
	}

#endif

	return AX_SUCCESS; 
}

void ax_free_data(
	AX_IN AX_DATA_NODE 		*node
){
	free(node->value);
}

