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
	_in const c16 		*substr
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (substr == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	c16 *chunk = axmalloc(IO_FILE_CHUNK);

	u64 start_off = 0;

	const c16 *chunk_loc = nullptr;
	const c16 *str_loc = nullptr;

	u64 init_off = file->offset;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);

		if (str_loc != nullptr){
			res = starts_with(chunk, str_loc, &str_loc);
		}

		// Either substr continuation not found or its the initial iteration
		if (str_loc == nullptr
		|| res == AX_NOT_FND){
			res = find_substr(chunk, substr, &chunk_loc, &str_loc);
			start_off = file->offset + dif_b(chunk, chunk_loc);
		}

		// Neither check found substr (skip chunk)
		if (res == AX_NOT_FND){
			str_loc = nullptr;
		}

		file->offset += IO_FILE_CHUNK;

		// Found
		if (res == AX_SUCC
		&& *str_loc == L'\0'){
			break;
		}

		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);

	file->offset = start_off;

	return AX_SUCC;
}

axres skip_while_f(
	_in io_file		*file,
	_in const c16		*charset
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (charset == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	c16 *chunk = axmalloc(IO_FILE_CHUNK);
	const c16 *chunk_loc = nullptr;

	u64 init_off = file->offset;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);
		res = skip_while(chunk, charset, &chunk_loc);
		axcheck_b(res);

		file->offset += dif_b(chunk, chunk_loc);

		// Found
		if (contains(charset, *chunk_loc) != AX_SUCC){
			break;
		}

		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);

	return AX_SUCC;
}
axres skip_until_f(
	_in io_file		*file,
	_in const c16		*charset
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (charset == nullptr){
		return AX_INV_ARG;
	}

	axres res = 0;

	c16 *chunk = axmalloc(IO_FILE_CHUNK);
	const c16 *chunk_loc = nullptr;

	u64 init_off = file->offset;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);
		res = skip_until(chunk, charset, &chunk_loc);
		if (res == AX_NOT_FND){
			// Last chunk character
			chunk_loc = &chunk[(IO_FILE_CHUNK - 1) / sizeof(c16)];
		} else axcheck_b(res);

		file->offset += dif_b(chunk, chunk_loc);

		// Found
		if (contains(charset, *chunk_loc) == AX_SUCC){
			break;
		}

		memset(chunk, 0x0, IO_FILE_CHUNK);
	} while(file->offset < file->size);

	axfree(chunk);
	axcheck(res, file->offset = init_off);

	return AX_SUCC;
}

axres skip_line_f(
	_in io_file		*file
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}

	axres res = AX_SUCC;

	res = skip_until_f(file, CHARSET_NL);
	axcheck(res);

	res = skip_while_f(file, CHARSET_NL);
	axcheck(res);

	return AX_SUCC;
}

axres skip_word_f(
	_in io_file		*file
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}

	axres res = AX_SUCC;

	res = skip_until_f(file, CHARSET_WS);
	axcheck(res);

	res = skip_while_f(file, CHARSET_WS);
	axcheck(res);

	return AX_SUCC;
}
