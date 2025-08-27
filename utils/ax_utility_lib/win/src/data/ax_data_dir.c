#include "ax_data.h"
#include "ax_error.h"
#include <stdio.h>
#include <wchar.h>
#include <errno.h>

axres read_data_dir(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
){
	return AX_SUCC;
}
axres write_data_dir(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	if (hdl == nullptr
	|| buf == nullptr){
		return AX_INV_ARG;
	}

	if (DATA_HANDLE_V(hdl)){
		ax_log_msg(AX_INV_DATA, L"Invalid handle content.");
		return AX_INV_DATA;
	}
	if (chkf(hdl->con.rule, URI_RULE_WRITE) == false){
		return AX_ACC_DEN;	
	}

	errno_t err = 0;
	FILE *file = malloc(sizeof(FILE));
	const c16 *mode = chkf(hdl->con.rule, URI_RULE_CREATE)
		? L"w"
		: L"r+";

	err = _wfopen_s(&file, hdl->con.user_data, mode);
	if (err != 0){
		return AX_INV_DATA;
	}

	fwrite(buf, size, 1, file);
	fclose(file);

	return AX_SUCC;
}

axres open_data_dir(
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
	if (URI_V(URI_DIR, uri, path)){
		return AX_INV_DATA;
	}

	hdl->ops = &_ops_dir; 
	
	hdl->con.id = CON_DIR;
	hdl->con.path = _wcsdup(path);
	hdl->con.rule = rule;
	hdl->con.is_open = true;
	hdl->con.data = nullptr;

	return AX_SUCC;
}
axres close_data_dir(
	_in data_handle 	*hdl
){
	return AX_SUCC;
}

