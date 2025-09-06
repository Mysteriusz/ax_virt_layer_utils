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

	u64 sub_start = 0; // Offset of the occurrence 

	const c16 *sub_char = substr;
	const c16 *occ = nullptr;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		if (AX_ERR(res)){
			break;
		}

		res = find_substr(chunk, sub_char, &occ, &sub_char);
		if (res != AX_NOT_FND 
		&& AX_ERR(res)){
			break;
		}
		// Not found in the currently processed substr
		// Reset the search
		if (res == AX_NOT_FND){ 
			sub_char = substr;			
			file->offset += IO_FILE_CHUNK;
			continue;
		}

		if (sub_start == 0){
			sub_start = file->offset + dif_b(chunk,occ); // Move by file offset and substr root 
		}
		file->offset += IO_FILE_CHUNK;
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	file->offset = 0;

	if (occ == nullptr
	|| AX_ERR(res)){
		return res;
	}

	*file_off = sub_start;

	return AX_SUCC;
}

