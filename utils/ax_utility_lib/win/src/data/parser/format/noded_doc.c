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

	ax_list *seq_list =  nullptr;
	ax_list_init(&seq_list);

	// Capture all section sequences
	res = seq_find_all_f(doc->file, NODED_SECT_FMT, seq_list);
	axcheck(res,
		seq_list->delete(seq_list),
		noded_doc_unload(doc) // doc core loaded by now
	);

	u64 beg_off = 0;
	u64 end_off = 0;
	seq_loc *curr = nullptr; 

	u64 value_len = 0;
	c16 *value = nullptr;

	noded_sect *sect = nullptr;

	for (u32 i = 0; i < seq_list->count; i++){
		curr = index_as(seq_list, i, seq_loc*);

		beg_off = dif_c16(doc->file->map.root, curr->beg);
		end_off = dif_c16(doc->file->map.root, curr->end);
		value_len = dif_c16(beg_off, end_off + 1);

		res = read_range(doc->file->map.root, beg_off, end_off, &value_len, nullptr);
		axcheck_b(res);

		value = axmalloc(value_len * sizeof(c16));

		res = read_range(doc->file->map.root, beg_off, end_off, &value_len, value);
		axcheck_b(res, axfree(value));

		res = noded_sect_load(doc, value, &sect);
		axfree(value);
		axcheck_b(res);
	}

	// Cleanup
	seq_list->delete(seq_list);
	axfree(sect);

	axcheck(res,
		noded_doc_unload(doc) // doc core loaded by now
	);

	// Document write-back
	*buf = doc;

	return AX_SUCC;
}

iter_code noded_doc_sect_unload_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_sect *sect = stack->node->value;
	noded_sect_unload(sect);

	return AX_SUCC;
}
axres noded_doc_unload(
	_in noded_doc		*doc
){
	if (noded_doc_inv(doc)){
		return AX_INV_ARG;
	}

	io_fc(doc->file);

	// Cleanup noded_sect list
	doc->sect_list->iter(
		doc->sect_list,
		(ax_iter_act)noded_doc_sect_unload_iter,
		nullptr,
		nullptr
	);
	doc->sect_list->delete(doc->sect_list);

	axfree(doc);

	return AX_SUCC;
}

