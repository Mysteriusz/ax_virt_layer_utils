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
	res = io_fo(path, IO_FILE_R, IO_FILE_ENC | IO_FILE_MAP, &doc->file);
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

	// Reset doc->file pointer
	io_foff(doc->file, _enc_size(doc->file->enc));

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

iter_code noded_doc_save_iter(
	_in ax_dict_iter_stack 		stack _prepass
){
	noded_sect *sect = (noded_sect*)stack->node->value;
	io_file *file = (io_file*)stack->data;
	if (file == nullptr){
		return ITER_FAIL;
	}

	axres res = AX_SUCC;

	c16 *sect_buf = nullptr;
	u32 sect_len_n = 0;
	res = noded_sect_c16(sect, &NODED_KVP_DEF, &sect_len_n, sect_buf);
	axcheck_r(res, ITER_FAIL);

	sect_buf = axmalloc(sect_len_n * sizeof(c16));

	res = noded_sect_c16(sect, &NODED_KVP_DEF, &sect_len_n, sect_buf);
	axcheck_r(res, ITER_FAIL, axfree(sect_buf));

	res = io_fw(file, (sect_len_n - 1) * sizeof(c16), sect_buf, nullptr);
	axfree(sect_buf);
	axcheck_r(res, ITER_FAIL);

	return ITER_NONE;
}
axres noded_doc_save(
	_in noded_doc		*doc
){
	if (noded_doc_inv(doc)){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	/*
	 	Create a temp file as a buffer
	*/
	io_file *temp = nullptr;
	res = io_fo_tmp(&temp);
	axcheck(res);

	res = doc->sect_dict->iter(
		doc->sect_dict,
		(ax_iter_act)noded_doc_save_iter,
		temp,
		nullptr
	);
	io_str(temp->path);
	io_fc(temp);

	axcheck(res);

	return AX_SUCC;
}

