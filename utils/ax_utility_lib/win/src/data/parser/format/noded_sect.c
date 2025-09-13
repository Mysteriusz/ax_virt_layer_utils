#include "noded.h"

axres noded_load_sect(
	_in noded_doc		*doc,
	_in const c16		*sect_name
){
	if (doc == nullptr
	|| sect_name == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;
	unref(res);

	return AX_SUCC;
}

