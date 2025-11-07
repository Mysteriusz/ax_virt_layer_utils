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

	/*
	 	Read line into an internal buffer
	*/
	u32 kvp_len = 0;
	c16 *kvp_buf = nullptr;

	res = read_line(kvp_loc, &kvp_len, kvp_buf);
	axcheck(res);

	kvp_buf = axmalloc(kvp_len * sizeof(c16));

	res = read_line(kvp_loc, &kvp_len, kvp_buf);
	axcheck(res);
	
	seq_loc inv_loc = {0};
	seq_find(kvp_buf, NODED_KVP_FMT, &inv_loc);

	return AX_SUCC;

	return res;
}
axres noded_kvp_unload(
	_in noded_kvp		*kvp
){
	return AX_SUCC;
}


