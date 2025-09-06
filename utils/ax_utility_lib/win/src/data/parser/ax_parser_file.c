#include "ax_parser.h"

axres find_substr_f(
	_in io_file		*file,
	_in const c16 		*substr,
	_out u64		*file_off // File offset where the occurrence is 
){
	if (io_file_inv(file)){
		return AX_INV_FILE;
	}
	if (substr == nullptr){
		return AX_INV_ARG;
	}
	if (file_off == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	c16 *chunk = axmalloc(IO_FILE_CHUNK);

	const c16 *occ = nullptr;
	const c16 *sub_occ = nullptr;

	u64 read = 0;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, &read);
		if (AX_ERR(res)){
			break;
		}

		res = find_substr(chunk, substr, &occ, &sub_occ);
		if (res != AX_NOT_FND 
		&& AX_ERR(res)){
			break;
		}

		// Read depending if end of the chunk was partially substr
		file->offset += (sub_occ != nullptr)
			? IO_FILE_CHUNK - dif_b(substr, sub_occ) // Read less by amount of bytes that occurred
			: IO_FILE_CHUNK; // No string part in the chunk
	} while(occ == nullptr
 	&& file->offset < file->size);

	io_str(occ);

	axfree(chunk);
	if (occ == nullptr
	|| AX_ERR(res)){
		return res;
	}

	return AX_SUCC;
}

