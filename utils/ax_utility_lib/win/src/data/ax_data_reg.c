#include "ax_data.h"

axres ops_read_reg(
	_in data_handle		hdl,	
	_out u32		size,
	_in_out void		*buf
){
	return AX_SUCC;
}

axres open_data_reg(
	_in c16			*uri,
	_in_out data_handle	*hdl
){
	if (uri == nullptr){
		return AX_INV_ARG;
	}
	if (hdl == nullptr){
		return AX_INV_BUF;
	}

	if (starts_with(uri, URI_REG) != AX_SUCC){
		return AX_INV_DATA;
	}
	
	axres res = AX_SUCC;

	const c16 *path = nullptr;	

	res = skip_word(uri, URI_REG, &path);
	if (AX_ERR(res)){
		return res;
	}
	
	hdl->con.id = CON_REG;
	hdl->con.data = malloc(10); // TODO: Interface for KM allocations for different systems
	hdl->ops = &_ops_reg; 

	return AX_SUCC;
}
axres close_data_reg(
	_in data_handle 	*hdl
){
	if (hdl == nullptr){
		return AX_INV_ARG; 
	}

	free(hdl->con.data);

	return AX_SUCC;
}

