#include "noded.h"

bool noded_doc_inv(
	_in noded_doc 		*doc
){
	if (doc == nullptr){
		return true;
	}
	if (io_finv(doc->file, UTF16)){
		return true;
	}

	return false;
}

void noded_load_sym_iter(
	const ax_list list _prepass,
	const ax_list_node node _prepass
){
	noded_sect *sect = (noded_sect*)node->value;
	ax_list_delete(sect->kvp);
	axfree(sect->label);

	return;
}
axres noded_load_sym(
	_in noded_doc 		*doc
){
	if (noded_doc_inv(doc)){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	ax_list *sect_list = nullptr;
	ax_list_init(&sect_list);
	ax_list *sym_list = nullptr;
	ax_list_init(&sym_list);

	res = seq_find_all_f(doc->file, NODED_SECT_FMT, sym_list);
	axcheck(res);

	// Add noded sects based on the sect sequence find
	noded_sect curr = {0};
	u64 label_off = 0;
	u64 label_len = 0;
	c16 *label_buf = nullptr;

	// Iterate over section sequences
	for (u64 i = 0; i < sym_list->count; i++){
		label_off = dif_b(doc->file->map.root, *index_as(sym_list, i, c16**));
		doc->file->offset = label_off;

		// Read label
		res = read_line_f(doc->file, &label_len, nullptr);
		axcheck_b(res);

		label_buf = axmalloc(label_len * sizeof(c16));

		res = read_line_f(doc->file, &label_len, label_buf);
		axcheck_b(res, axfree(label_buf));

		// Load into sect buffer
		curr.label = label_buf;
		curr.offset = label_off;

		// Add sect buffer to list
		res = sect_list->add(sect_list, &curr, sizeof(noded_sect));
		axcheck_b(res);
	}

	// Cleanup sect_list if failed
	axcheck(res, 
		sect_list->iter(sect_list, (ax_structures_iter_act)noded_load_sym_iter);
		sect_list->delete(sect_list);
	);

	doc->sect_list = sect_list;

	return AX_SUCC;
}

