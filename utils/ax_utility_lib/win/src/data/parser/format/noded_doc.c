#include "noded.h"

axres noded_load_doc(
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
	axcheck(res, axfree(doc), axfree(file));

	res = noded_load_sym(doc);
	axcheck(res,
		io_fc(file),
		axfree(doc),
		axfree(file)
	);

	*buf = doc;

	return AX_SUCC;
}

void noded_unload_iter(
	const ax_list list _prepass,
	const ax_list_node node _prepass
){
	noded_sect *sect = (noded_sect*)node->value;
	// TODO: Cleanup sect->kvp
	axfree(sect->label);

	return;
}
axres noded_unload_doc(
	_in noded_doc		**doc
){
	if (doc == nullptr
	|| noded_doc_inv(*doc)){
		return AX_INV_ARG;
	}

	(*doc)->sect_list->iter((*doc)->sect_list, (ax_structures_iter_act)noded_unload_iter);
	io_fc((*doc)->file);
	axfree(*doc);

	return AX_SUCC;
}

