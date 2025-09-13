#include "noded.h"

axres noded_init_doc(
	_in const c16		*path,
	_out noded_doc		*doc
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (doc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	io_file *root_f = axmalloc(sizeof(io_file));

	res = io_fo(path, IO_FILE_R, root_f);
	axcheck(res);

	res = noded_load_sym(doc);
	axcheck(res,
		io_fc(root_f);
		axfree(root_f);
	);

	return AX_SUCC;
}

