#include "noded.h"

void noded_doc_on_clear(
	ax_dict_node 		node _prepass
){
	c16 *name = node->key;
	noded_sect *sect = node->value;
	noded_sect_unload(sect);

	axfree(name);
	axfree(sect);
}
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

	// Initialize sequence list for the section capture
	ax_list *seq_list = nullptr;
	ax_list_init(&seq_list);

	// Capture all section sequences
	res = seq_find_all_f(doc->file, NODED_SECT_FMT, true, seq_list);
	axcheck_g(res, cleanup);

	// Initialize section dicionary of the doc
	ax_dict_init(seq_list->count, &doc->sect_dict);
	doc->sect_dict->overrides.on_clear = (ax_structure_override)noded_doc_on_clear;

	seq_loc *curr = nullptr; 

	// Iterate all occurences and load them into doc->sect_list
	for (u32 i = 0; i < seq_list->count; i++){
		curr = index_as(seq_list, seq_loc*, i);
		axcheck_b((curr == nullptr));

		// Load sect into the doc
		res = noded_sect_load(doc, curr);
		axcheck_b(res);
	}

cleanup:
	// Cleanup
	seq_list->delete(seq_list);
	axcheck(res,
		noded_doc_unload(doc) // doc core loaded by now
	);

	// Document write-back
	*buf = doc;
	return AX_SUCC;
}

axres noded_doc_unload(
	_in noded_doc		*doc
){
	if(doc == nullptr){
		return AX_INV_ARG;
	}

	doc->sect_dict->delete(doc->sect_dict);
	io_fc(doc->file);
	axfree(doc);

	return AX_SUCC;
}

