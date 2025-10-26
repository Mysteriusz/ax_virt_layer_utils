#include "noded.h"

axres noded_sect_load(
	_in noded_doc		*doc,
	_in const c16		*sect_loc
){
	if (noded_doc_inv(doc)){
		return AX_INV_FILE;
	}
	if (sect_loc == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	noded_sect sect = {0};

	// Allocated
	u64 name_len = 0;
	c16 *name_buf = nullptr;
	u64 rng_len = 0;
	c16 *rng_buf = nullptr;

	sect_loc++;

	// Read name of the section
	res = read_until(sect_loc, L"]", &name_len, name_buf);
	axcheck(res);

	name_buf = axmalloc(name_len * sizeof(c16));

	res = read_until(sect_loc, L"]", &name_len, name_buf);
	axcheck_g(res, error_jump);

	// Skip to section range
	res = skip_line(sect_loc, &sect_loc);
	axcheck_g(res, error_jump);

	// Skip to next section location
	seq_loc next_loc = {0};
	res = seq_find(sect_loc, NODED_SECT_FMT, &next_loc);
	if (res == AX_NOT_FND){
		// If none found skip to doc->file->map.root end
		next_loc.beg = sect_loc + _c16len(sect_loc);
	}else axcheck_g(res, error_jump);

	// Read section KVP range
	res = read_range(
		sect_loc,
		0,
		dif_c16(sect_loc, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck_g(res, error_jump);

	rng_buf = axmalloc(rng_len * sizeof(c16));

	res = read_range(
		sect_loc,
		0,
		dif_c16(sect_loc, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck_g(res, error_jump);

	const c16 *rng_char = rng_buf;

	ax_list_init(&sect.kvp_list);

	do{
		/*res = noded_kvp_load(sect, rng_char, &kvp_buf);
		axcheck_b(res);

		sect->kvp_list->add(
			sect->kvp_list,
			&kvp_buf,
			sizeof(noded_kvp*)
		);*/
		io_str(L"LINE");
	}while(skip_line(rng_char, &rng_char) == AX_SUCC);
	
	axcheck_g(res, error_jump);
	axfree(rng_buf);

	sect.name = name_buf;
	sect.doc = doc;
	sect.beg = sect_loc;
	sect.end = next_loc.beg;

	doc->sect_list->add(doc->sect_list, &sect, sizeof(noded_sect));

	return AX_SUCC;

error_jump:
	axfree(rng_buf);
	axfree(name_buf);
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
	io_str(sect->doc->file->path);
	if (noded_sect_inv(sect)){
		return AX_INV_ARG;
	}

	sect->kvp_list->iter(
		sect->kvp_list,
		(ax_iter_act)noded_unload_sect_iter,
		nullptr,
		nullptr
	);
	sect->kvp_list->delete(sect->kvp_list);
	axfree(sect->name);

	return AX_SUCC;
}

