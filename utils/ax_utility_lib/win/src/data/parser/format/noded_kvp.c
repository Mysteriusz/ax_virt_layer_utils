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

	io_str(u"=========KURWA NAME=========");
	io_str(index_as(loc.seq_vars, c16*, u"kvp_name", sizeof(u"kvp_name")));
	io_str(u"=========KURWA VALUE=========");	
	io_str(index_as(loc.seq_vars, c16*, u"kvp_val", sizeof(u"kvp_val")));

	//axfree(kvp_buf);
	axcheck(res);

	return AX_SUCC;
}
axres noded_kvp_unload(
	_in noded_kvp		*kvp
){
	return AX_SUCC;
}


