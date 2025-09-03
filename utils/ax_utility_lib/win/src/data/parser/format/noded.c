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

	u64 sect_off = 0;	
	res = noded_find_sect(&file, sect_name, &sect_off);
	if (AX_ERR(res)){
		io_fc(&file);	
		return res;
	}

	io_fc(&file);	

	return AX_SUCC;
}
axres noded_find_sect(
	_in io_file		*file,
	_in const c16		*sect_name, 
	_out u64		*offset
){
	if (file == nullptr){
		return AX_INV_FILE;
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
	u32 label_size_b = label_size * sizeof(c16); 

	if (label_size_b > NODED_SECT_BOUND){
		return AX_BUF_TOO_BIG;
	}

	c16 *label = axmalloc(label_size_b);
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

	u64 read = 0;
	c16 *data = nullptr;
	const c16 *occ = nullptr;

	// Read chunks until found
	do{
		if (data != nullptr){
			axfree(data);
		}

		data = axmalloc(NODED_SEARCH_CHUNK);
		
		res = io_fr(file, NODED_SEARCH_CHUNK, data, &read);
		if (AX_ERR(res)){
			axfree(data);
			return res;
		}
		file->offset += NODED_SEARCH_CHUNK - label_size_b;
	} while(find_substr(data, label, &occ) == AX_NOT_FND
	&& file->offset < file->size);

	file->offset = 0;

	if (occ == nullptr){
		return AX_NOT_FND;
	}

	io_str(occ);

	return AX_SUCC;
}

