#include "noded.h"

axres noded_kvp_load(
	_in noded_sect		*sect,
	_in const c16		*kvp_loc,
	_out noded_kvp		**kvp
){
	if (sect == nullptr
	|| kvp_loc == nullptr){
		return AX_INV_ARG;
	}
	if (kvp == nullptr){
		return AX_INV_BUF;
	}

	return AX_SUCC;
}

