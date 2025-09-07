#include "ax_parser.h"

axres check_ext(
	_in const c16 		*path,
	_in const c16		*ext
){
	if (path == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	const c16 *path_ext = nullptr;
	res = skip_until(path, L".", &path_ext);
	axcheck(res);

	path_ext++;

	return compare(path_ext, ext);
}
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

	u64 init_off = file->offset;
	u64 sub_start = 0; // Offset of the occurrence 

	const c16 *sub_char = substr;
	const c16 *occ = nullptr;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);

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
			sub_start = file->offset + dif_b(chunk,occ); // Move by file and chunk offset
		}
		file->offset += IO_FILE_CHUNK;
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	file->offset = init_off;

	if (occ == nullptr
	|| AX_ERR(res)){
		return res;
	}

	*file_off = sub_start;

	return AX_SUCC;
}

axres skip_until_f(
	_in io_file		*file,
	_in const c16		*charset,
	_out u64		*file_off
){
	if (io_file_inv(file)){
		return AX_INV_FILE;
	}
	if (charset == nullptr){
		return AX_INV_ARG;
	}
	if (file_off == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	c16 *chunk = axmalloc(IO_FILE_CHUNK);

	u64 init_off = file->offset;
	u64 occ_off = 0; // Offset of the occurrence 

	const c16 *occ = nullptr;
	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);

		res = skip_until(chunk, charset, &occ);
		if (res != AX_NOT_FND
		&& AX_ERR(res)){
			break;
		}else if (res == AX_NOT_FND){
			file->offset += IO_FILE_CHUNK;
		}else{ // Found
			occ_off = file->offset + dif_b(chunk,occ); // Move by file and chunk offset 
			break;
		}
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	file->offset = init_off;
	axcheck(res);
	
	*file_off = occ_off;

	return AX_SUCC;
}

axres skip_line_f(
	_in io_file		*file,
	_out u64		*file_off
){
	if (io_file_inv(file)){
		return AX_INV_FILE;
	}
	if (file_off == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 nl_off = 0;
	res = skip_until_f(file, CHARSET_NL, &nl_off);
	axcheck(res);

	// Skip the found CHARSET_NL character
	nl_off += _enc_size(file->enc);

	*file_off = nl_off;

	return AX_SUCC;
}

