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
	if (_enc_size(file->enc) != 2){ // CHECK UTF-16 !!!TEMP!!!
		return AX_INV_ENC;
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
	u64 occ_off = 0; // Offset of the occurrence 

	do{
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);

	file->offset = occ_off;
	*file_off = occ_off;

	return AX_SUCC;
}

axres skip_while_f(
	_in io_file		*file,
	_in const c16		*charset,
	_out u64		*file_off
){
	if (io_file_inv(file)){
		return AX_INV_FILE;
	}
	if (_enc_size(file->enc) != 2){ // CHECK UTF-16 !!!TEMP!!!
		return AX_INV_ENC;
	}
	if (charset == nullptr){
		return AX_INV_ARG;
	}
	if (file_off == nullptr){
		return AX_INV_BUF;
	}

	axres res = 0;
	c16 *chunk = axmalloc(IO_FILE_CHUNK);

	u64 init_off = file->offset;
	u64 occ_off = 0; // Offset of the occurrence 

	const c16 *occ = nullptr;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);

		res = skip_while(chunk, charset, &occ);
		if (res != AX_NOT_FND
		&& AX_ERR(res)){ // Critical error
			break;
		}else if (contains(charset, *occ) == AX_SUCC){ // Continue search
			file->offset += IO_FILE_CHUNK;
		}else{ // Found
		        // Move by file and chunk offset 
			occ_off = (file->offset == 0) /* TODO: Kurwa refactor */\
				? file->offset + (dif_b(chunk,occ) + _enc_size(file->enc)) \
				: file->offset + dif_b(chunk,occ); \
			break;
		}
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);
	
	file->offset = occ_off;
	*file_off = occ_off;

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
	if (_enc_size(file->enc) != 2){ // CHECK UTF-16 !!!TEMP!!!
		return AX_INV_ENC;
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
		&& AX_ERR(res)){ // Critical error
			break;
		}else if (res == AX_NOT_FND){ // Continue search
			file->offset += IO_FILE_CHUNK;
		}else{ // Found
		        // Move by file and chunk offset 
			occ_off = (file->offset == 0) /* TODO: Kurwa refactor */\
				? file->offset + (dif_b(chunk,occ) + _enc_size(file->enc)) \
				: file->offset + dif_b(chunk,occ); \
			break;
		}
		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);
	
	file->offset = occ_off;
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

	// Skip the found 0x0a character
	nl_off += _enc_size(file->enc);
	file->offset = nl_off; 
	*file_off = nl_off;

	return AX_SUCC;
}

axres skip_word_f(
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
	res = skip_until_f(file, CHARSET_WS, &nl_off);
	axcheck(res);

	file->offset = nl_off; 
	*file_off = nl_off;

	return AX_SUCC;
}
