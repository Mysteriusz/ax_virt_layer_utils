#include "noded.h"

bool noded_doc_inv(
	_in noded_doc 		*doc
){
	if (doc == nullptr){
		return true;
	}
	if (io_finv(doc->file, 0)){
		return true;
	}

	return false;
}

axres noded_load_sym(
	_in noded_doc 		*doc
){
	if (noded_doc_inv(doc)){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;
	unref(res);

	return AX_SUCC;
}

