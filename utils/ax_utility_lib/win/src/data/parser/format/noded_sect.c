#include "noded.h"

iter_code noded_load_sect_iter(
	ax_list_iter_stack 	stack _prepass
){
	noded_sect *sect = (noded_sect*)stack->node->value;
	if (sfmemcmp(
		sect->label,
		stack->data, 
		_c16len_b(sect->label), 
		_c16len_b(stack->data)) == 0
	){
		return ITER_STOP;
	}

	return ITER_NONE;
}

axres noded_load_sect(
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

	axres res = AX_SUCC;

	c16 *sect_label = nullptr;
	u64 sect_label_len = 0;

	// Join into sect
	res = join_with(sect_label, &sect_label_len, 3,
		NODED_SECT_BEG, sect_name, NODED_SECT_END);
	axcheck(res);

	sect_label = axmalloc(sect_label_len * sizeof(c16));

	res = join_with(sect_label, &sect_label_len, 3,
		NODED_SECT_BEG, sect_name, NODED_SECT_END);
	axcheck(res, axfree(sect_label));

	// Iterate to find if exists as symbol
	const ax_list_node *node = nullptr;
	doc->sect_list->iter(
		doc->sect_list,
		(ax_iter_act)noded_load_sect_iter,
		sect_label,
		(const void**)&node
	);

	// Cleanup
	axfree(sect_label);
	if (node == nullptr){
		return AX_NOT_FND;
	}

	noded_sect *sect_sym = (noded_sect*)node->value;

	// Write-back
	*sect = sect_sym;

	return AX_SUCC;
}

