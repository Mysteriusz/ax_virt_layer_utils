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
	_in const c16		*sect_name,
	_out noded_sect		**sect
){
	if (noded_doc_inv(doc)){
		return AX_INV_FILE;
	}
	if (sect_name == nullptr){
		return AX_INV_ARG;
	}
	if (sect == nullptr){
		return AX_INV_BUF;
	}

	/*axres res = AX_SUCC;

	c16 *fmt = nullptr;
	u64 fmt_len = 0;
	res = join_with(fmt, &fmt_len, 0, FMT_GRP, L"[", sect_name, L"]:", FMT_GRP);*/

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

