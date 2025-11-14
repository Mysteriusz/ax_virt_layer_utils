#include "noded.h"

void noded_sect_on_clear(
	ax_dict_node 		node _prepass
){
	c16 *name = node->key;
	noded_kvp *kvp = node->value;
	noded_kvp_unload(kvp);

	axfree(name);
	axfree(kvp);
}

// Resolve (read) range of the section
_free c16 *_noded_sect_resolve(
	_in noded_doc		*doc,
	_in seq_loc		*sect_loc
){
	if (sect_loc == nullptr){
		return nullptr;
	}

	axres res = AX_SUCC;

	// Skip initial line
	const c16 *sect_char = sect_loc->end;
	seq_loc next_loc = {0};
	res = seq_find(sect_char, NODED_SECT_FMT, &next_loc);
	if (res == AX_NOT_FND){
		next_loc.beg = doc->file->map.root + doc->file->map.size;
	}else axcheck_r(res, nullptr);

	// If next section/EOF has no offset between
	if (dif_c16(sect_char, next_loc.beg) == 0){
		return axmalloc(1);
	}

	/*
	 	Read range of the section
	*/
	u32 rng_len_n = 0;
	c16 *rng_buf = nullptr;
	res = read_range(
		sect_char,
		0, 
		dif_c16(sect_char, next_loc.beg),
		&rng_len_n, rng_buf
	);
	axcheck_r(res, nullptr);

	rng_buf = axmalloc(rng_len_n * sizeof(c16));

	res = read_range(
		sect_char,
		0, 
		dif_c16(sect_char, next_loc.beg),
		&rng_len_n, rng_buf
	);
	axcheck_r(res, nullptr, axfree(rng_buf));

	return rng_buf;
}
axres noded_sect_load(
	_in noded_doc		*doc,
	_in seq_loc		*sect_loc
){
	if (noded_doc_inv(doc)){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	noded_sect sect = {0};

	// Read sect_name value
	const c16 *name = index_as(sect_loc->seq_vars, c16*, u"sect_name", sizeof(u"sect_name"));
	axcheck_r((name == nullptr), AX_INV_DATA);

	// Read section range (until next section or EOF)
	c16 *rng_buf = _noded_sect_resolve(doc, sect_loc);
	axcheck_r((rng_buf == nullptr), AX_INV_DATA);
	u32 rng_len = _c16len(rng_buf);

	const c16 *rng_char = rng_buf;

	/*
	 	Write to buffer before processing lines 
	*/
	sect.name = _c16dup(name);
	sect.doc = doc;
	sect.beg = rng_buf;
	sect.end = rng_buf + rng_len;

	// Initialize kvp dicionary of the section
	u32 kvp_dict_max = 0; // Maximum count of nodes (if each line is a valid kvp node)
	line_count(rng_buf, &kvp_dict_max);
	ax_dict_init(kvp_dict_max, &sect.kvp_dict);
	sect.kvp_dict->overrides.on_clear = (ax_structure_override)noded_sect_on_clear;

	/*
		Process each line as kvp
	*/
	while(in_c16_s(rng_buf, rng_char, rng_len)){
		res = noded_kvp_load(&sect, rng_char);
		axcheck_g(res, error_jump);

		res = skip_line(rng_char, &rng_char);
		axcheck_b(res);
	}
	axcheck(res, sect.kvp_dict->delete(sect.kvp_dict), axfree(rng_buf));

	doc->sect_dict->add(
		doc->sect_dict,
		(void*)name,
		_c16len_b(name) + sizeof(c16),
		&sect,
		sizeof(noded_sect)
	);

	axfree(rng_buf);
	return AX_SUCC;

error_jump:
	axfree(rng_buf);

	return res;
}

axres noded_sect_unload(
	_in noded_sect 		*sect
){
	if(sect == nullptr){
		return AX_INV_ARG;
	}

	sect->kvp_dict->delete(sect->kvp_dict);
	axfree(sect->name);

	return AX_SUCC;
}

