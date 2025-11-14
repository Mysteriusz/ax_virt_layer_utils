#include "noded.h"

axres noded_kvp_load(
	_in noded_sect		*sect,
	_in const c16		*kvp_loc
){
	if (noded_sect_inv(sect)){
		return AX_INV_DATA;
	}
	if (kvp_loc == nullptr){
		return AX_INV_ARG;
	}
	
	axres res = AX_SUCC;

	noded_kvp kvp = {0};

	/*
	 	Read line into an internal buffer
	*/
	u32 kvp_len_n = 0;
	c16 *kvp_buf = nullptr;

	res = read_line(kvp_loc, &kvp_len_n, kvp_buf);
	axcheck(res);

	// Empty line check
	if (kvp_len_n == 1){
		return AX_SUCC;
	}

	kvp_buf = axmalloc(kvp_len_n * sizeof(c16));

	res = read_line(kvp_loc, &kvp_len_n, kvp_buf);
	axcheck(res, axfree(kvp_buf));

	/*
	 	Load initial metadata
	*/
	kvp.sect = sect;
	kvp.beg = kvp_loc;
	kvp.end = kvp_loc + (kvp_len_n - 1);

	/*
	 	Parse kvp as sequence
	*/
	seq_loc loc = {0};
	ax_dict_init(3, &loc.seq_vars);

	res = seq_find(kvp_buf, NODED_KVP_FMT, &loc);
	axcheck(res,
		loc.seq_vars->delete(loc.seq_vars),
		axfree(kvp_buf)
	);

	/*
	 	Read values from the dictionary
	*/
	const c16 *name = index_as(loc.seq_vars, c16*, u"kvp_name", sizeof(u"kvp_name"));
	const c16 *value = index_as(loc.seq_vars, c16*, u"kvp_val", sizeof(u"kvp_val"));
	if (name == nullptr
	|| value == nullptr){
		axcheck(res,
			loc.seq_vars->delete(loc.seq_vars),
			axfree(kvp_buf)
		);
	}

	kvp.name = _c16dup(name);
	kvp.value = _c16dup(value);

	sect->kvp_dict->add(
		sect->kvp_dict,
		kvp.name,
		_c16len_b(kvp.name) + sizeof(c16),
		&kvp,
		sizeof(noded_kvp)
	);

	/*
	 	Cleanup
	*/
	loc.seq_vars->delete(loc.seq_vars);
	axfree(kvp_buf);

	return AX_SUCC;
}
axres noded_kvp_unload(
	_in noded_kvp		*kvp
){
	axfree(kvp->name);
	axfree(kvp->value);

	return AX_SUCC;
}


