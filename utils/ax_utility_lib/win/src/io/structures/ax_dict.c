#include "ax_dict.h"

axres ax_dict_init(
	_in u32				exp_max, // Maximum expected count of values
	_out ax_dict			**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	ax_dict *dict = axmalloc(sizeof(ax_dict));
	dict->type = structure_seq;
	dict->root = nullptr;
	dict->count = 0;
	dict->bucket_count = (exp_max * 4 + 3 - 1) / 3; // Around ~0.75 factor

	dict->add = (ax_structures_add_kv)ax_dict_add;

	*buf = dict;

	return AX_SUCC;
}

axres ax_dict_add(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size,
	_in void 			*value,
	_in u64 			value_size
){
	return AX_SUCC;
}

