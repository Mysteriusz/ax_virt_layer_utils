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

	data_reg_desc *desc = axmalloc(sizeof(data_reg_desc));
	desc->type = type;
	desc->name = _wcsdup(name);

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
	if (desc->name != nullptr){
		axfree(desc->name);
	}
	axfree(desc);

	return AX_SUCC;
}

axres read_data_reg(
	_in data_handle		*hdl,	
	_in_out u32		*size,
	_in_out _eval void	*buf
){
	bool ret_size = false;
	axres res = read_data_inv(hdl, size, buf, &ret_size);
	axcheck(res);

#if defined(AX_UM)
	LSTATUS stat = ERROR_SUCCESS;
	data_reg_desc *desc = ((data_reg_desc*)(hdl->con.user_data));

	// Query needed buffer size
	DWORD fsize = 0;
	stat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		desc->name,
		nullptr,
		nullptr,
		nullptr,
		&fsize
	);
	// Validate buffer size 
	res = _ax_buf_err(fsize, *size);
	// Check size and return correct one 
	if (AX_ERR(res)
	&& ret_size){
		*size = fsize;
		return AX_SUCC;
	} else axcheck(res);

	stat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		desc->name,
		nullptr,
		(DWORD*)&desc->type,
		buf,
		(LPDWORD)size
	);

	if (stat != ERROR_SUCCESS){
		ax_log_lstat(stat);
		return AX_INV_DATA;
	}
#endif // defined(AX_UM)

	return AX_SUCC;
}
axres write_data_reg(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	axres res = write_data_inv(hdl, size, buf);
	axcheck(res);

#if defined(AX_UM)
	LSTATUS lstat = ERROR_SUCCESS;
	data_reg_desc *desc = ((data_reg_desc*)(hdl->con.user_data));

	// Existence check
	lstat = RegQueryValueExW(
		(HKEY)hdl->con.data,
		desc->name,
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
			desc->name,
			0,
			desc->type,
			buf,
			size
		);
	}

	if (lstat != ERROR_SUCCESS){
		ax_log_lstat(lstat);
		return AX_INV_DATA;
	}
#endif // defined(AX_UM)

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

	axres res = AX_SUCC;

	const c16 *path = nullptr;
	if (URI_V(URI_REG, uri, path)){
		return AX_INV_DATA; 
	}

	hdl->ops = &_ops_reg; 
	
	hdl->con.id = CON_REG;
	hdl->con.path = _wcsdup(path);
	hdl->con.rule = rule;
	hdl->con.is_open = true;

	res = con_reg_data(hdl, &hdl->con.data);
	axcheck(res,
		axfree(hdl->con.path),
		memset(hdl, 0x00, sizeof(data_handle))
	);

	return AX_SUCC;
}

axres close_data_reg(
	_in data_handle 	*hdl
){
	if (data_handle_inv(hdl)){
		return AX_INV_ARG; 
	}

	axfree(hdl->con.path);
#if defined(AX_UM)
	CloseHandle((HKEY)hdl->con.data);
#elif defined(AX_KM)
	ZwClose((HANDLE)hdl->con.data);
#endif

	return AX_SUCC;
}

