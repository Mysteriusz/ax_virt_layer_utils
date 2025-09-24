#include "ax_data.h"
#include "ax_error.h"

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

	u64 size = 0;

	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	axcheck(res);

	path = axmalloc(size * sizeof(c16));
	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	axcheck(res, axfree(path));

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

	axfree(hdl->con.user_data);
	return AX_SUCC;
}

axres read_data_dir(
	_in data_handle		*hdl,	
	_in_out u32		*size,
	_in_out _eval void	*buf
){
	bool ret_size = false;
	axres res = read_data_inv(hdl, size, buf, &ret_size);
	axcheck(res);

	io_file	file = {0};	

	res = io_fo(hdl->con.user_data, IO_FILE_R, &file);
	axcheck(res);

	u64 fsize = 0;
	res = io_fsize(file.path, &fsize);
	axcheck(res);

	// Validate buffer size 
	res = _ax_buf_err(fsize, *size);
	// Check size and return correct one 
	if (AX_ERR(res)
	|| ret_size){
		*size = astp(u32,fsize);
		io_fc(&file);
		return AX_SUCC;
	}

	res = io_fr(&file, *size, buf, nullptr);
	axcheck(res);

	io_fc(&file);

	return AX_SUCC;
}
axres write_data_dir(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	axres res = write_data_inv(hdl, size, buf);
	axcheck(res);

	io_file file = {0};

	res = io_fo(hdl->con.user_data, rule_to_io(hdl->con.rule), &file);
	axcheck(res);

	res = io_fw(&file, size, buf, nullptr);
	axcheck(res);

	io_fc(&file);

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
	if (data_handle_inv(hdl)){
		return AX_INV_ARG;
	}

	axfree(hdl->con.path);

	return AX_SUCC;
}

