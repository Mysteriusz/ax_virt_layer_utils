#include "ax_data.h"
#include "ax_data_reg_h.c"

axres push_data_reg(
	_in data_handle 	*hdl,
	_in u32			type,
	_in const c16		*name
){
	if (hdl == nullptr
	|| name == nullptr){
		return AX_INV_ARG;
	}

	data_reg_desc *desc = malloc(sizeof(data_reg_desc));
	desc->dwType = type;
	desc->lpValueName = _wcsdup(name);

	hdl->con.user_data = desc;

	return AX_SUCC;
}
axres pop_data_reg(
	_in data_handle 	*hdl
){
	if (hdl == nullptr
	|| hdl->con.user_data == nullptr){
		return AX_INV_ARG;
	}

	data_reg_desc *desc = (data_reg_desc*)hdl->con.user_data;
	free((void*)desc->lpValueName);
	free(desc);

	return AX_SUCC;
}

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
	
	if (data_handle_inv(hdl)){
		return AX_INV_DATA;
	}
	if (chkf(hdl->con.rule, URI_RULE_READ) == false){
		return AX_ACC_DEN;	
	}

	LSTATUS stat = ERROR_SUCCESS;
	data_reg_desc *desc = ((data_reg_desc*)(hdl->con.user_data));

	stat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		desc->lpValueName,
		nullptr,
		&desc->dwType,
		buf,
		(LPDWORD)size
	);

	if (stat != ERROR_SUCCESS){
		ax_log_lstat(stat);
		return AX_INV_DATA;
	}

	return AX_SUCC;
}
axres write_data_reg(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	if (hdl == nullptr
	|| buf == nullptr){
		return AX_INV_ARG;
	}

	if (data_handle_inv(hdl)){
		ax_log_msg(AX_INV_DATA, L"Invalid handle content.");
		return AX_INV_DATA;
	}
	if (chkf(hdl->con.rule, URI_RULE_WRITE) == false){
		return AX_ACC_DEN;	
	}

	LSTATUS lstat = ERROR_SUCCESS;
	data_reg_desc *desc = ((data_reg_desc*)(hdl->con.user_data));

	// Existence check
	lstat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		desc->lpValueName,
		0,
		nullptr,
		nullptr,
		nullptr	
	);
	if ((lstat == ERROR_FILE_NOT_FOUND
	&& chkf(hdl->con.rule, URI_RULE_CREATE))
	|| lstat == ERROR_SUCCESS){
		lstat = RegSetValueExW(
			(HKEY)hdl->con.data,
			desc->lpValueName,
			0,
			desc->dwType,
			buf,
			size
		);
	}

	if (lstat != ERROR_SUCCESS){
		ax_log_lstat(lstat);
		return AX_INV_DATA;
	}

	return AX_SUCC;
}

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

	const c16 *path = nullptr;
	if (URI_V(URI_REG, uri, path)){
		return AX_INV_DATA; 
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

