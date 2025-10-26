#include "noded.h"

iter_code noded_load_sect_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_sect *sect = (noded_sect*)stack->node->value;
	if (sfmemcmp(
		sect->name,
		stack->data, 
		_c16len_b(sect->name), 
		_c16len_b(stack->data)) == 0
	){
		return ITER_STOP;
	}

	return ITER_NONE;
}

axres noded_sect_load(
	_in noded_doc		*doc,
	_in const c16		*sect_loc,
	_out noded_sect		**sect
){
	if (noded_doc_inv(doc)){
		return AX_INV_FILE;
	}
	if (sect == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	res = skip_line(sect_loc, &sect_loc);
	axcheck(res);

	// Skip to next section location
	seq_loc next_loc = {0};
	res = seq_find(sect_loc, NODED_SECT_FMT, &next_loc);
	if (res == AX_NOT_FND){
		// If none found skip to doc->file->map.root end
		next_loc.beg = sect_loc + _c16len(sect_loc);
	}else axcheck(res);

	// Read section KVP range
	u64 rng_len = 0;
	c16 *rng_buf = nullptr;
	res = read_range(
		sect_loc,
		0,
		dif_c16(sect_loc, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck(res);

	rng_buf = axmalloc(rng_len * sizeof(c16));

	res = read_range(
		sect_loc,
		0,
		dif_c16(sect_loc, next_loc.beg), 
		&rng_len,
		rng_buf
	);
	axcheck(res, axfree(rng_buf));

	const c16 *rng_char = rng_buf;
	noded_kvp kvp = {0};

	do{
		res = noded_kvp_load(rng_char, &kvp);
		axcheck_b(res);
	}while(skip_line(rng_char, &rng_char) == AX_SUCC);
	
	axfree(rng_buf);
	return AX_SUCC;
}
axres noded_sect_unload(
	_in noded_sect 		*sect
){
	if (noded_sect_inv(sect)){
		return AX_INV_ARG;
	}

	axfree(sect->name);

	return AX_SUCC;
}

