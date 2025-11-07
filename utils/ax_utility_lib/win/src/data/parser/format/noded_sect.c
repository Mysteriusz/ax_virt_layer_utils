#include "noded.h"

axres noded_sect_load(
	_in noded_doc		*doc,
	_in seq_loc		sect_loc,
	_in ax_dict 		*sect_vars
){
	if (noded_doc_inv(doc)){
		return AX_INV_DATA;
	}
	if (sect_vars == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	noded_sect sect = {0};

	u32 rng_len = 0;
	c16 *rng_buf = nullptr;

	const c16 *sect_char = sect_loc.beg;
	const c16 *name = index_as(sect_vars, c16*, u"sect_name", sizeof(u"sect_name"));
	if (name == nullptr){
		return AX_INV_DATA;
	}

	// Skip to section range
	res = skip_line(sect_char, &sect_char);
	axcheck_g(res, error_jump);

	// Skip to next section location
	seq_loc next_loc = {0};
	res = seq_find(sect_char, NODED_SECT_FMT, &next_loc);
	if (res == AX_NOT_FND){
		// If none found skip to doc->file->map.root end
		next_loc.beg = sect_char + _c16len(sect_char);
	}else axcheck_g(res, error_jump);

	/*
	   	Read section KVP range
	*/
	res = read_range(
		sect_char,
		0,
		dif_c16(sect_char, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck_g(res, error_jump);

	rng_buf = axmalloc(rng_len * sizeof(c16));

	res = read_range(
		sect_char,
		0,
		dif_c16(sect_char, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck_g(res, error_jump);

	const c16 *rng_char = rng_buf;

	ax_list_init(&sect.kvp_list);

	// Iterate ranges and load into sect
	do{
		//res = noded_kvp_load(&sect, rng_char);
		axcheck_b(res);
	}while(skip_line(rng_char, &rng_char) == AX_SUCC);
	
	axcheck_g(res, error_jump);
	axfree(rng_buf);

	sect.name = _c16dup(name);
	sect.doc = doc;
	sect.beg = sect_char;
	sect.end = next_loc.beg;

	doc->sect_list->add(
		doc->sect_list,
		&sect,
		sizeof(noded_sect)
	);

	return AX_SUCC;

error_jump:
	axfree(rng_buf);
	noded_sect_unload(&sect);

	return res;
}

iter_code noded_unload_sect_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_kvp **kvp = stack->node->value;
	noded_kvp_unload(*kvp);

	return ITER_NONE;
}
axres noded_sect_unload(
	_in noded_sect 		*sect
){
	if(sect == nullptr){
		return AX_INV_ARG;
	}

	if (sect->kvp_list != nullptr){
		sect->kvp_list->iter(
			sect->kvp_list,
			(ax_iter_act)noded_unload_sect_iter,
			nullptr,
			nullptr
		);
		sect->kvp_list->delete(sect->kvp_list);
	}
	axfree(sect->name);

	return AX_SUCC;
}

