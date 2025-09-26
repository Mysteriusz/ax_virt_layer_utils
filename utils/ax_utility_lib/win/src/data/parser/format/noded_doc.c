#include "noded.h"

axres noded_init_doc(
	_in const c16		*path,
	_out noded_doc		**buf
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	noded_doc *doc = axmalloc(sizeof(noded_doc));
	io_file *file = axmalloc(sizeof(io_file));
	doc->file = file;

	res = io_fo(path, IO_FILE_R, file);
	axcheck(res, axfree(file));

	res = noded_load_sym(doc);
	axcheck(res,
		io_fc(file);
		axfree(file);
	);

	*buf = doc;

	return AX_SUCC;
}

