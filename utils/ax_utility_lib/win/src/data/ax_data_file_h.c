#include "ax_data.h"

axres con_file_data(
	_in data_handle		*hdl,
	_out void		**buf
){
	if (hdl == nullptr){
		return AX_INV_ARG; 
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	io_file *file = axmalloc(sizeof(io_file));

#if defined(AX_UM)

	res = io_fo(hdl->con.path, rule_to_io(hdl->con.rule), file); 
	axcheck(res);

#elif defined(AX_KM)
	unref(res);
#endif

	*buf = file;

	return AX_SUCC;
}

