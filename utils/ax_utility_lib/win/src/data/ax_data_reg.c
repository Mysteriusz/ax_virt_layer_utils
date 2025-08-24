#include "ax_data.h"

axres read_data_reg(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
){
	if (size == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (hdl == nullptr){
			return AX_INV_ARG;
		}

		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}
	
	if (hdl->con.data == nullptr
	|| hdl->con.user_data == nullptr){
		return AX_INV_DATA;
	}

	LSTATUS stat = ERROR_SUCCESS;

	stat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		(c16*)hdl->con.user_data,
		nullptr,
		nullptr,
		buf,
		(LPDWORD)size
	);

	if (stat != ERROR_SUCCESS){
		ax_log_lstat(stat);
		return AX_INV_DATA;
	}

	return AX_SUCC;
}

#include "ax_data_reg_h.c"

axres open_data_reg(
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

	if (starts_with(uri, URI_REG) != AX_SUCC){
		return AX_INV_DATA;
	}
	
	axres res = AX_SUCC;

	const c16 *path = nullptr;	

	res = skip_word(uri, URI_REG, &path);
	if (AX_ERR(res)){
		return res;
	}

	hdl->ops = &_ops_reg; 
	
	hdl->con.id = CON_REG;
	hdl->con.path = _wcsdup(path);
	hdl->con.rule = rule;
	hdl->con.is_open = true;
	hdl->con.data = _con_reg_data(hdl);

	return AX_SUCC;
}
axres close_data_reg(
	_in data_handle 	*hdl
){
	if (hdl == nullptr){
		return AX_INV_ARG; 
	}

	if (hdl->con.path != nullptr){
		free(hdl->con.path);
	}

	CloseHandle((HKEY)hdl->con.data);

	return AX_SUCC;
}

