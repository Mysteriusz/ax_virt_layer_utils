#include "noded.h"

axres noded_load_sect(
	_in noded_doc		*doc,
	_in const c16		*sect_name
){
	if (noded_doc_inv(doc)){
		return AX_INV_FILE;
	}
	if (sect_name == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	c16 *sect_full = nullptr;
	u64 sect_full_s = 0;

	res = join_with(sect_full, &sect_full_s, 3,
		NODED_SECT_BEG, sect_name, NODED_SECT_END);
	axcheck(res);

	sect_full = axmalloc(sect_full_s * sizeof(c16));

	res = join_with(sect_full, &sect_full_s, 3,
		NODED_SECT_BEG, sect_name, NODED_SECT_END);
	axcheck(res, axfree(sect_full));

	res = find_substr_f(doc->file, sect_full);
	axfree(sect_full);
	axcheck(res);

	return AX_SUCC;
}

