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

	// fmap with file offset
	const c16 *fmap_off = file->map.root + file->offset;
	// Location of substr in fmap
	const c16 *loc = file->map.root;

	res = find_substr(fmap_off, substr, &loc, nullptr);
	axcheck(res);

	file->offset = dif_b(file->map.root, loc);

	return AX_SUCC;
}
axres seq_find_f(
	_in io_file		*file,
	_in const c16 		*fmt
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (fmt == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	// fmap with file offset
	const c16 *fmap_off = file->map.root + file->offset;
	// Location of substr in fmap
	seq_loc buf = {0};

	res = seq_find(fmap_off, fmt, &buf);
	axcheck(res);

	file->offset = (u64)buf.beg;

	return AX_SUCC;
}
axres seq_find_all_f(
	_in io_file		*file,
	_in const c16 		*fmt,
	_in_out ax_list		*locs
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (fmt == nullptr){
		return AX_INV_ARG;
	}
	if (locs == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	// fmap with file offset
	const c16 *fmap_off = file->map.root;

	res = seq_find_all(fmap_off, fmt, locs);
	axcheck(res);

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

	// fmap with file offset
	const c16 *fmap_off = file->map.root + file->offset;
	// Location of substr in fmap
	const c16 *loc = file->map.root;

	res = skip_while(fmap_off, charset, &loc);
	axcheck(res);

	file->offset = dif_b(file->map.root, loc);

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

	axres res = AX_SUCC;

	// fmap with file offset
	const c16 *fmap_off = file->map.root + file->offset;
	// Location of substr in fmap
	const c16 *loc = file->map.root;

	res = skip_until(fmap_off, charset, &loc);
	axcheck(res);

	file->offset = dif_b(file->map.root, loc);

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

axres read_line_f(
	_in io_file		*file,
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	
	axres res = AX_SUCC;

	u64 line_len = *size;
	const c16 *file_off = file->map.root + file->offset;

	res = read_line(file_off, &line_len, buf);
	axcheck(res);

	*size = line_len;
	return AX_SUCC;
}

