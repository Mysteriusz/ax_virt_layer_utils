#include "ax_data.h"

#if defined(AX_WIN32)

#define HIVE_CU			L"HKEY_CURRENT_USER"
#define HIVE_U			L"HKEY_USERS"
#define HIVE_LM			L"HKEY_LOCAL_MACHINE"
#define HIVE_CR			L"HKEY_CLASSES_ROOT"
#define HIVE_CC			L"HKEY_CURRENT_CONFIG"

HKEY res_hive(
	_in const c16		*value
){
	if (value == nullptr){
		return null;
	}

	if (compare(value, HIVE_CU) == AX_SUCC){
		return HKEY_CURRENT_USER;
	}else if (compare(value, HIVE_U) == AX_SUCC){
		return HKEY_USERS;
	}else if (compare(value, HIVE_LM) == AX_SUCC){
		return HKEY_LOCAL_MACHINE;
	}else if (compare(value, HIVE_CR) == AX_SUCC){
		return HKEY_CLASSES_ROOT;
	}else if (compare(value, HIVE_CC) == AX_SUCC){
		return HKEY_CURRENT_CONFIG;
	}
	else{
		return null;
	}
}
HKEY res_path(
	_in c16			**path_d
){
	if (path_d == nullptr){
		return null;
	}
	
	HKEY hive = res_hive(path_d[0]);
	unref(hive);

	return hive;
}

static void *_con_reg_data(
	_in const c16		*path
){
	if (path == nullptr){
		return null;
	}

	c16 **path_d = nullptr; 
	u32 path_s = 0; 

	split_by(path, L"\\/", &path_s, path_d);  
	path_d = malloc(path_s * sizeof(c16*));
	split_by(path, L"\\/", &path_s, path_d);  

	HKEY t = res_path(path_d);
	printf("%p\n", t);

	c_split_by(path_d, path_s);

	return (void*)t;
}

#endif

