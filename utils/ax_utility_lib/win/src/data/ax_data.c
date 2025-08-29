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

bool data_handle_inv(
	_in data_handle		*hdl
){
	// Validate handle pointer
	if (hdl == nullptr){
		return true;
	}
	// Validate hdl->ops
	if (hdl->ops == nullptr
	|| hdl->ops->read == nullptr
	|| hdl->ops->write == nullptr){
		return true;
	}

	// Validate hdl->con
	// con.data CAN be nullptr ONLY 
	// when con.id is CON_DIR since it doesnt store any data other than con.path 
	bool con_data_v = (hdl->con.id == CON_DIR)
		? (hdl->con.data == nullptr)
		: (hdl->con.data != nullptr);

	if (hdl->con.path == null
	|| hdl->con.is_open == false // Valid only when open
	|| !con_data_v){
		return true;
	}

	return false; // valid
}

