#include "ax_data.h"

axres con_file_data(
	_in data_handle		*hdl,
	_out FILE		**buf
){
	if (hdl == nullptr){
		return AX_INV_ARG; 
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	// Try to find appropriate file mode for the rule
	c16 *mode = nullptr;
	res = find_eval_node(RULE_EVAL_N, RULE_EVAL(hdl->con.rule), &mode); 
	if (AX_ERR(res)){
		ax_log_msg(res, L"Invalid file mode evaluation for handle rule.");
		return AX_INV_DATA;
	}

	// Try to open/create the file depening on rule
	FILE *file = nullptr;
	errno_t err = _wfopen_s(&file, hdl->con.path, mode); 
	if (err != 0){
		ax_log_errno(err);
		return AX_INV_DATA;
	}

	*buf = file;

	return AX_SUCC;
}

