#include "ax_data.h"
#include "ax_error.h"
#include "ax_data_file_h.c"

axres open_data_file(
	_in c16			*uri,
	_in i64			rule,
	_in_out data_handle	*hdl
){
	if (uri == nullptr){
		return AX_INV_ARG;
	}
	if (hdl == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	const c16 *path = nullptr;
	if (URI_V(URI_FILE, uri, path)){
		return AX_INV_DATA;
	}

	hdl->ops = &_ops_file; 
	
	hdl->con.id = CON_FILE;
	hdl->con.path = _wcsdup(path);
	hdl->con.rule = rule;
	hdl->con.is_open = true;

	res = con_file_data(hdl, (FILE**)&hdl->con.data);
	if (AX_ERR(res)){
		free(hdl->con.path);
		memset(hdl, 0x00, sizeof(data_handle));
		return res;
	}

	return AX_SUCC;
}
axres close_data_file(
	_in data_handle 	*hdl
){
	if (data_handle_inv(hdl)){
		return AX_INV_ARG;
	}

	free(hdl->con.path);

	return AX_SUCC;
}

axres read_data_file(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out _eval void	*buf
){
	return AX_SUCC;
}
axres write_data_file(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	return AX_SUCC;
}

