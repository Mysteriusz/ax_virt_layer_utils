#include "ax_data.h"
#include "ax_error.h"
#include <stdio.h>
#include <wchar.h>
#include <errno.h>

axres push_data_dir(
	_in data_handle 	*hdl,
	_in const c16		*ext,
	_in const c16		*name
){
	if (hdl == nullptr
	|| ext == nullptr
	|| name == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;
	c16 *path = nullptr;
	u32 size = 0;

	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	if (AX_ERR(res)){
		return res;
	}
	path = malloc(size * sizeof(c16));
	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	if (AX_ERR(res)){
		free(path);
		return res;
	}

	hdl->con.user_data = path;

	return AX_SUCC;
}
axres pop_data_dir(
	_in data_handle 	*hdl
){
	if (hdl == nullptr
	|| hdl->con.user_data == nullptr){
		return AX_INV_ARG;
	}

	free(hdl->con.user_data);
	return AX_SUCC;
}

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

	if (data_handle_inv(hdl)){
		ax_log_msg(AX_INV_DATA, L"Invalid handle content.");
		return AX_INV_DATA;
	}
	if (chkf(hdl->con.rule, URI_RULE_WRITE) == false){
		return AX_ACC_DEN;	
	}

	errno_t err = 0;
	FILE *file = malloc(sizeof(FILE));
	const c16 *mode = chkf(hdl->con.rule, URI_RULE_CREATE)
		? L"w" // Read/Write Create
		: L"r+"; // Read/Write !Create

	err = _wfopen_s(&file, hdl->con.user_data, mode);

	// If file did not exist and 	
	// lacked write access (URI_RULE_CREATE)
	if (err == ENOENT
	&& compare(mode, L"r+") == AX_SUCC){
		return AX_ACC_DEN;
	} else if (err != 0){
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

