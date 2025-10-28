#include "noded.h"

axres noded_doc_load(
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

	// Initialize document
	noded_doc *doc = axmalloc(sizeof(noded_doc));
	res = io_fo(path, IO_FILE_R, &doc->file);
	axcheck(res, axfree(doc));

	ax_list_init(&doc->sect_list);

	ax_list *seq_list = nullptr;
	ax_list_init(&seq_list);

	// Capture all section sequences
	res = seq_find_all_f(doc->file, NODED_SECT_FMT, seq_list);
	axcheck(res,
		seq_list->delete(seq_list),
		noded_doc_unload(doc) // doc core loaded by now
	);

	seq_loc *curr = nullptr; 

	//io_i64(seq_list->count);
	// Iterate all occurences and load them into doc->sect_list
	for (u32 i = 0; i < seq_list->count; i++){
		curr = index_as(seq_list, i, seq_loc*);
		axcheck_b((curr == nullptr));

		// Load sect into the doc
		res = noded_sect_load(doc, *curr);
		axcheck_b(res);
	}

	// Cleanup
	seq_list->delete(seq_list);
	axcheck(res,
		noded_doc_unload(doc) // doc core loaded by now
	);

	// Document write-back
	*buf = doc;

	return AX_SUCC;
}

iter_code noded_doc_unload_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_sect *sect = stack->node->value;

	io_str(sect->name);
	noded_sect_unload(sect);

	return ITER_NONE;
}
axres noded_doc_unload(
	_in noded_doc		*doc
){
	if(doc == nullptr){
		return AX_INV_ARG;
	}

	if (doc->sect_list != nullptr){
		// Cleanup noded_sect list
		doc->sect_list->iter(
			doc->sect_list,
			(ax_iter_act)noded_doc_unload_iter,
			nullptr,
			nullptr
		);
		doc->sect_list->delete(doc->sect_list);
	}

	io_fc(doc->file);
	axfree(doc);

	return AX_SUCC;
}

