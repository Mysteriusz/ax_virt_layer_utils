#include "noded.h"

axres noded_load_doc(
	_in const c16		*path,
	_out noded_doc		**doc
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (doc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	noded_doc *doc_buf = axmalloc(sizeof(noded_doc));
	io_file *file = axmalloc(sizeof(io_file));
	doc_buf->file = file;

	res = io_fo(path, IO_FILE_R, file);
	axcheck(res, axfree(doc_buf), axfree(file));

	res = noded_load_sym(doc_buf);
	axcheck(res,
		io_fc(file),
		axfree(doc_buf),
		axfree(file)
	);

	// Document write-back
	*doc = doc_buf;

	return AX_SUCC;
}

iter_code noded_unload_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_sect *sect = (noded_sect*)stack->node->value;
	// TODO: Cleanup sect->kvp
	axfree(sect->label);

	return ITER_NONE;
}
axres noded_unload_doc(
	_in noded_doc		**doc
){
	if (doc == nullptr
	|| noded_doc_inv(*doc)){
		return AX_INV_ARG;
	}

	(*doc)->sect_list->iter((*doc)->sect_list, (ax_iter_act)noded_unload_iter, nullptr, nullptr);
	io_fc((*doc)->file);
	axfree(*doc);

	return AX_SUCC;
}

