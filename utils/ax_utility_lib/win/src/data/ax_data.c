#include "ax_data.h"

axres open_data(
	_in c16			*uri,
	_in i64			rule,
	_out data_handle	*hdl
){
	if (uri == nullptr){
		return AX_INV_ARG;
	}
	if (hdl == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	
	if (starts_with(uri, URI_REG) == AX_SUCC){
		res = open_data_reg(uri, rule, hdl);
	}else if (starts_with(uri, URI_DIR) == AX_SUCC){
		res = open_data_dir(uri, rule, hdl);
	}else if (starts_with(uri, URI_FILE) == AX_SUCC){
		return AX_NOT_IMP;
	}else{
		return AX_INV_DATA;
	}

	if (AX_ERR(res)){
		return res;
	}

	return AX_SUCC;
}
axres close_data(
	_in data_handle 	*hdl
){
	if (hdl == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	if (hdl->con.id == CON_REG){
		res = close_data_reg(hdl);
	}else if (hdl->con.id == CON_DIR){
		res = close_data_dir(hdl);
	}else if (hdl->con.id == CON_FILE){
		return AX_NOT_IMP;
	}else{
		return AX_INV_DATA;
	}

	if (AX_ERR(res)){
		return res;
	}

	return AX_SUCC;
}

