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

#if defined(AX_UM)
	u32 size = 0;

	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	if (AX_ERR(res)){
		return res;
	}
	path = axmalloc(size * sizeof(c16));
	res = join_with(path, &size, 3, hdl->con.path, name, ext);
	if (AX_ERR(res)){
		axfree(path);
		return res;
	}
#elif defined(AX_KM)
	unref(res);
#endif

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
	if (AX_ERR(res)){
		return res;
	}

#if defined(AX_UM)
	axres err = 0;
	FILE *file = malloc(sizeof(FILE));

	err = _wfopen_s(&file, hdl->con.user_data, L"r");
	if (err == ENOENT){
		return AX_INV_DATA;
	} 

	// Get file size 
	fseek(file, 0, SEEK_END);
	u32 fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Validate buffer size 
	res = _ax_buf_err(fsize, *size);
	// Check size and return correct one 
	if (AX_ERR(res)
	&& ret_size){
		*size = fsize;
		return AX_SUCC;
	} else if (AX_ERR(res)){
		return res;
	}

	fread(buf, 1, *size, file);
	fclose(file);
#elif defined(AX_KM)
#endif

	return AX_SUCC;
}
axres write_data_dir(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
){
	axres res = write_data_inv(hdl, size, buf);
	if (AX_ERR(res)){
		return res;
	}

#if defined(AX_UM)
	errno_t err = 0;
	FILE *file = axmalloc(sizeof(FILE));
	const c16 *mode = chkf(hdl->con.rule, URI_RULE_CREATE)
		? L"w+" // Read/Write/Create
		: L"r+"; // Read/Write

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
#elif defined(AX_KM)
#endif

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

