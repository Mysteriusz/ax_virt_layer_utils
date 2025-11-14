#include "noded.h"

bool noded_doc_inv(
	_in noded_doc 		*doc
){
	if (doc == nullptr){
		return true;
	}
	if (io_finv(doc->file, UTF16)){
		return true;
	}
	if (doc->sect_dict == nullptr){
		return true;
	}

	return false;
}
bool noded_sect_inv(
	_in noded_sect 		*sect
){
	if (sect == nullptr){
		return true;
	}
	if (sect->doc == nullptr
	|| sect->name == nullptr){
		return true;
	}
	if (sect->kvp_dict == nullptr){
		return true;
	}
	if (noded_doc_inv(sect->doc)){
		return true;
	}

	return false;
}
bool noded_kvp_inv(
	_in noded_kvp 		*kvp
){
	if (kvp->value == nullptr
	|| kvp->name == nullptr){
		return true;
	}
	if (noded_sect_inv(kvp->sect)){
		return true;
	}

	return false;
}

