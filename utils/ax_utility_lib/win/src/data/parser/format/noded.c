#include "noded.h"

axres noded_load_sect(
	_in c16				*path,
	_out u32 			*size,
	_out noded_sect			*buf
){
	if (path == nullptr){
		return AX_INV_ARG;
	}

	if (size == nullptr
	|| buf == nullptr){
		return AX_INV_BUF;
	}

		

	return AX_SUCC;
}

