#include "ax_data.h"

#if defined(AX_WIN32)

#define HIVE_CU			L"HKEY_CURRENT_USER"
#define HIVE_U			L"HKEY_USERS"
#define HIVE_LM			L"HKEY_LOCAL_MACHINE"
#define HIVE_CR			L"HKEY_CLASSES_ROOT"
#define HIVE_CC			L"HKEY_CURRENT_CONFIG"

#define RULE_TO_SAM(rule) \
	(chkf(rule, URI_RULE_ADM) \
	 ? KEY_ALL_ACCESS \
	 : KEY_READ \
) 

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
	_in c16			**path_d,
	_in u32			count,
	_in i64			rule
){
	if (path_d == nullptr
	|| count == 0){
		return null;
	}
	
	HKEY hive = res_hive(path_d[0]);

	HKEY buf = hive; 
	LSTATUS stat = ERROR_SUCCESS;

	u32 dwSamDesired = RULE_TO_SAM(rule);
	for (u32 i = 1; i < count; i++){
		if (chkf(rule, URI_RULE_CREATE)){
			stat = RegCreateKeyExW(
				buf,
				path_d[i],
				0,
				null,
				0,
				dwSamDesired, 
				nullptr,
				&buf,
				nullptr
			);
		}
		else{
			stat = RegOpenKeyExW(
				buf,
				path_d[i],
				0,
				dwSamDesired, 
				&buf
			);
		}

		if (stat != ERROR_SUCCESS){
			ax_log_lstat(stat);
			return null;
		}
	}

	return buf;
}

static void *_con_reg_data(
	_in data_handle		*hdl
){
	if (hdl == nullptr
	|| hdl->con.path == nullptr){
		return null;
	}

	c16 **path_d = nullptr; 
	u32 path_s = 0; 

	split_by(hdl->con.path, L"\\/", &path_s, path_d);  
	path_d = malloc(path_s * sizeof(c16*));
	split_by(hdl->con.path, L"\\/", &path_s, path_d);  

	HKEY t = res_path(path_d, path_s, hdl->con.rule);

	c_split_by(path_d, path_s);
	free(path_d);

	return (void*)t;
}

#endif

