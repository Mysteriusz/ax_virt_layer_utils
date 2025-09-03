#include "noded.h"

axres noded_load_sect(
	_in const c16		*path,
	_in const c16		*sect_name, 
	_out noded_sect		**sect
){
	if (path == nullptr){
		return AX_INV_ARG;
	}

	if (sect == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	const c16 *ext = nullptr;
	res = skip_until(path, L".", &ext);
	if (AX_ERR(res)){
		return res;
	}

	/*
		Skip the L'.' -> path\\to\\file\\file.noded
						      ^
	*/
	ext++;

	if (compare(ext, L"noded") != AX_SUCC){
		return AX_INV_FILE;	
	}

	io_file file = {0};
	res = io_fo(path, IO_FILE_R, &file);
	if (AX_ERR(res)){
		return res;
	}

	u32 sect_size = 0;	
	c16 *sect_root = nullptr;
	noded_find_sect(&file, sect_name, &sect_size, (void**)&sect_root);

	io_fc(&file);	

	return AX_SUCC;
}
axres noded_find_sect(
	_in io_file		*file,
	_in const c16		*sect_name, 
	_out u32		*sect_size,
	_out void		**sect_root
){
	if (file == nullptr){
		return AX_INV_FILE;
	}
	
	if (sect_size == nullptr
	|| sect_root == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	/*
	 	Allocate size for the label.
		Example:
			[my_sect_name]:
	*/
	u32 label_size = (
	 	_c16len(NODED_SECT_END)
		+ _c16len(sect_name)
		+ _c16len(NODED_SECT_END)
		+ _c16len(NODED_SECT_PTR)
		+ 1 // null-terminator
	);
	c16 *label = axmalloc(label_size * sizeof(c16));

	res = join_with(
		label,
		&label_size,
		4,
		NODED_SECT_SET,
		sect_name,
		NODED_SECT_END,
		NODED_SECT_PTR
	); 
	if (AX_ERR(res)){
		return res;
	}

	//c16 *data = axmalloc(NODED_SEARCH_CHUNK);
	c16 *data = nullptr;
	u64 read = 0;
	// Read first chunk
	res = io_fr(file, NODED_SEARCH_CHUNK, data, &read);

	ax_log(res);
	ax_log(read);

	return AX_SUCC;
}

