#include "noded.h"

axres noded_load_sect(
	_in const c16		*path,
	_in const c16		*sect_name, 
	_out noded_sect		**sect
){
	if (path == nullptr
	|| sect_name == nullptr){
		return AX_INV_ARG;
	}
	if (sect == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	res = check_ext(path, NODED_EXT);
	axcheck(res);

	io_file file = {0};
	res = io_fo(path, IO_FILE_R, &file);
	axcheck(res);

	res = noded_find_sect(&file, sect_name);
	axcheck(res);

	u64 sect_size = 0;
	res = noded_size_sect(&file, file.offset, &sect_size);
	axcheck(res);

	io_fc(&file);

	return AX_SUCC;
}
axres noded_size_sect(
	_in io_file		*file,
	_in u64 		sect_off,
	_out u64		*sect_size
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (sect_size == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	unref(res);
	return AX_SUCC;
}
axres noded_find_sect(
	_in io_file		*file,
	_in const c16		*sect_name
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (sect_name == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	u32 sect_label_s = 0;
	c16 *sect_label = nullptr;

	res = noded_label_sect(sect_name, &sect_label_s, sect_label); 
	axcheck(res);

	sect_label = axmalloc(sect_label_s * sizeof(c16));

	res = noded_label_sect(sect_name, &sect_label_s, sect_label); 
	axcheck(res, axfree(sect_label));

	res = find_substr_f(file, sect_label);

	axfree(sect_label);
	axcheck(res);

	return AX_SUCC;
}
axres noded_label_sect(
	_in const c16		*name,
	_out u32 		*size,
	_in_out _eval c16	*buf // Evaluate by (size * sizeof(c16))
){
	if (name == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	/*
	 	Allocate size for the label.
		Example:
			[my_sect_name]:
	*/
	u32 label_size = (
	 	_c16len(NODED_SECT_END)
		+ _c16len(name)
		+ _c16len(NODED_SECT_END)
		+ _c16len(NODED_SECT_PTR)
		+ 1 // null-terminator
	);
	u32 label_size_b = label_size * sizeof(c16); 
	if (label_size_b > NODED_SECT_BOUND){
		return AX_BUF_TOO_BIG;
	}

	if (ret_size){
		*size = label_size;
		return AX_SUCC;
	}

	axres res = AX_SUCC;
	res = join_with(
		buf,
		&label_size,
		4,
		NODED_SECT_SET,
		name,
		NODED_SECT_END,
		NODED_SECT_PTR
	);
	axcheck(res);

	return AX_SUCC;
}

