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

	// File offset of the first substr occcurence.
	u64 start_off = 0;

	const c16 *chunk_loc = nullptr; // Location in chunk*
	const c16 *str_loc = nullptr; // Location in substr*

	u64 init_off = file->offset;

	do{
		res = io_fr(file, IO_FILE_CHUNK, chunk, nullptr);
		axcheck_b(res);

/*
		Check continuation after substr was cut by chunk		 	

		Example:
			- previous chunk = L"some_s"
			- previous str_loc = L"some_string"
			Here the part of the string was found in the chunk.
			From now on each other will pass through here and move str_loc further.

			- chunk = L"tring other_string"
			- str_loc = L"tring"

			And so on until either:
				- Chunk is not a continuation of the string.
				- String was found.
*/
		if (str_loc != nullptr){
			res = starts_with(chunk, str_loc, &str_loc);
		}

/*
 		Either the continuation of string not found or its the initial read.
		If substr continuation not found it will rescan the chunk the string.
*/
		if (str_loc == nullptr
		|| res == AX_NOT_FND){
			res = find_substr(chunk, substr, &chunk_loc, &str_loc);
			start_off = file->offset + dif_b(chunk, chunk_loc);
		}

		// Chunk does not have substr (restart)
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
axres find_sequence_f(
	_in io_file		*file,
	_in const c16 		*seq
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (seq == nullptr){
		return AX_INV_ARG;
	}



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
