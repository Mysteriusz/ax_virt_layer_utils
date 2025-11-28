#include "ax_data.h"

#if defined(AX_WIN64)

#define RULE_TO_SAM(rule) \
	(chkf(rule, URI_RULE_ADM) \
	 ? KEY_ALL_ACCESS \
	 : KEY_READ \
) 

#if defined(AX_UM)

#define HIVE_CU			u"HKEY_CURRENT_USER"
#define HIVE_U			u"HKEY_USERS"
#define HIVE_LM			u"HKEY_LOCAL_MACHINE"
#define HIVE_CR			u"HKEY_CLASSES_ROOT"
#define HIVE_CC			u"HKEY_CURRENT_CONFIG"

void *res_hive(
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
#endif // defined(AX_UM)
void *res_path(
	_in data_handle 	*hdl
){
	if (hdl == nullptr){
		return nullptr;
	}

	axres res = AX_SUCC;
	HANDLE *buf = nullptr; 

#if defined(AX_UM)
	u32 path_len_n = 0;
	c16 **path_d = nullptr; 

	// Split path into keys to follow 
	res = split_by(hdl->con.path, u"\\/", &path_len_n, path_d);
	if (AX_ERR(res)){
		ax_log(res);
		return nullptr;
	}

	path_d = axmalloc(path_len_n * sizeof(c16*));

	res = split_by(hdl->con.path, u"\\/", &path_len_n, path_d);
	if (AX_ERR(res)){
		ax_log(res);
		return nullptr;
	}
	HANDLE hive = res_hive(path_d[0]);

	buf = hive; 
	LSTATUS stat = ERROR_SUCCESS;

	u32 dwSamDesired = RULE_TO_SAM(hdl->con.rule);
	for (u32 i = 1; i < path_len_n - 1; i++){
		if (chkf(hdl->con.rule, URI_RULE_CREATE)){
			stat = RegCreateKeyExW(
				(HKEY)buf,
				path_d[i],
				0,
				null,
				0,
				dwSamDesired, 
				nullptr,
				(PHKEY)&buf,
				nullptr
			);
		}
		else{
			stat = RegOpenKeyExW(
				(HKEY)buf,
				path_d[i],
				0,
				dwSamDesired, 
				(PHKEY)&buf
			);
		}

		if (stat != ERROR_SUCCESS){
			return null;
		}
	}
// TODO:
#elif defined(AX_KM)
	unref(res);
#endif 

	return buf;
}

axres con_reg_data(
	_in data_handle		*hdl,
	_out HANDLE 		*buf
){
	if (hdl == nullptr
	|| hdl->con.path == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	// Resolve key handle from array
	HANDLE t = res_path(hdl);
	if (t == null){
		ax_log_msg(AX_INV_DATA, u"Resolving registry path failed");
		return AX_INV_DATA;
	}

	*buf = t;

	return AX_SUCC;
}

#endif

