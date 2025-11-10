#if defined(AX_WIN64)

#include "ax_file.h"
#include "ax_parser.h"

#include <errhandlingapi.h>

#if defined(AX_UM)

bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_enc 	exp_enc // Pass expected encoding
){
	if (file == nullptr){
		return true;
	}

	if (file->path == nullptr){
		return true;
	}

	/*
	 	Read and invalidate encoding
	*/
	io_file_enc file_enc = 0;
	if (io_fbom(file->path, &file_enc) != AX_SUCC){
		return true;
	}
	if (exp_enc != 0
	&& _enc_size(file_enc) != _enc_size(exp_enc)){
		return true;
	}

	/*
	 	Reset pointer to offset
	*/
	if (!SetFilePointerEx(
		file->hdl,
		(LARGE_INTEGER){.QuadPart = file->offset},
		nullptr,
		0)
	){
		return true;
	}

	return false;
}

axres io_fbom(
	_in const c16		*path,
	_out io_file_enc	*buf
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	// Allocate buffer and read leading 4 bytes
	u32 *bom = axmalloc(4);

	// Open file
	HANDLE hdl = CreateFileW(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
		io_i64(GetLastError());
		res = AX_INV_PATH;
		goto error_jump;
	}

	if (!ReadFile(hdl, bom, 4, nullptr, nullptr)){
		res = AX_INV_BUF;
		goto error_jump;
	}

	io_file_enc enc = 0;

	// Read size CAN be UTF32
	if(memcmp(bom, addr(UTF32LE_BOM), 4) == 0){
		enc = UTF32LE;
	}else if(memcmp(bom, addr(UTF32BE_BOM), 4) == 0){
		enc = UTF32BE;
	}else if(memcmp(bom, addr(UTF8LE_BOM), 3) == 0){
		enc = UTF8LE;
	}else if(memcmp(bom, addr(UTF8BE_BOM), 3) == 0){
		enc = UTF8BE;
	}else if(memcmp(bom, addr(UTF16LE_BOM), 2) == 0){
		enc = UTF16LE;
	}else if(memcmp(bom, addr(UTF16BE_BOM), 2) == 0){
		enc = UTF16BE;
	}

	if(enc == 0){
		res = AX_INV_ENC;
		goto error_jump;
	}

	*buf = enc;

	axfree(bom);
	CloseHandle(hdl);
	return AX_SUCC;

error_jump:

	axfree(bom);
	CloseHandle(hdl);

	return res;
}
axres io_fex(
	_in const c16		*path	
){
	if (path == nullptr){
		return AX_INV_ARG;
	}

	// Open file
	HANDLE hdl = CreateFileW(
		path,
		FILE_READ_DATA,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
		return AX_INV_PATH;
	}
	CloseHandle(hdl);

	return AX_SUCC;
}
axres io_fsize(
	_in const c16		*path,
	_out u64		*size
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (size == nullptr){
		return AX_INV_BUF;
	}

	// Open file
	HANDLE hdl = CreateFileW(
		path,
		FILE_READ_DATA,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
		return AX_INV_PATH;
	}

	// Read size
	LARGE_INTEGER buf = {0};
	if (!GetFileSizeEx(hdl, &buf)){
		CloseHandle(hdl);
		return AX_INV_ARG;
	}
	
	CloseHandle(hdl);
	*size = buf.QuadPart;

	return AX_SUCC;
}
axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_out io_file		**buf
){
	if (path == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	/*
	 	Convert access to rights and disposal convention
	*/
	u32 rights = _io_file_conv_win64(acc);
	u32 disp = (chkf(rights, FILE_WRITE_DATA) == true) 
		? OPEN_ALWAYS
		: OPEN_EXISTING;

	io_file *file = axmalloc(sizeof(io_file));

	/*
		Open file
	*/
	HANDLE hdl = CreateFileW(
		path,
		rights,
		FILE_SHARE_READ,
		nullptr,
		disp,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
		res = AX_INV_DATA;
		goto error_jump;
	}

	/*
		Read the encoding
	*/
	res = io_fbom(path, &file->enc);
	axcheck_g(res, error_jump);

	/*
	 	Allocate the virtual map
	*/
	res = io_fmmap(hdl, true, &file->map);
	axcheck_g(res, error_jump);

	/*
		Write-back non-gathered data
	*/
	file->offset += _bom_size(file->enc);
	file->acc = acc;
	file->hdl = hdl;
	file->path = _c16dup(path);

	if (io_finv(file, file->enc)){
		io_fc(file);
		return AX_INV_FILE;
	}

	*buf = file;
	
	return AX_SUCC;

error_jump:

	io_funmap(&file->map);
	CloseHandle(hdl);
	axfree(file);

	return res;
}

void io_fc(
	_in io_file		*file
){
	io_funmap(&file->map);
	CloseHandle(file->hdl);
	axfree(file->path);
	axfree(file);
}

axres io_fr(
	_in io_file		*file,
	_in u32			size,
	_in_out void 		*buf,
	_out_opt u32		*read // Bytes read
){
	if (io_finv(file, file->enc)){
		return AX_INV_FILE;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	if (!chkf(file->acc, IO_FILE_R)){
		return AX_ACC_DEN;
	}

#if defined(AX_STRICT_BUF_SIZE)
	/*
		Check buf size against file size
	*/
	u64 file_size = 0;
	io_fsize(file->path, &file_size);

	if (size > file_size - file->offset){
		return AX_BUF_TOO_BIG;
	}
#endif
	
	/*
	 	Read file with provided buffer info
		Read can only fail due to invalid buf since file is invalidated
	*/
	DWORD read_count = 0;
	if (!ReadFile(file->hdl, buf, size, &read_count, nullptr)){
		return AX_INV_BUF;
	}

	file->offset += read_count;
	if (read != nullptr){
		*read = read_count;
	}

	return AX_SUCC;
}
axres io_fw(
	_in io_file		*file,
	_in u32			size,
	_in void 		*buf,
	_out_opt u32		*writ // Bytes written
){
	if (io_finv(file, file->enc)){
		return AX_INV_FILE;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	if (!chkf(file->acc, IO_FILE_W)){
		return AX_ACC_DEN;
	}

	/*
	 	Read file with provided buffer info
		Read can only fail due to invalid buf since file is invalidated
	*/
	DWORD writ_count = 0;
	if (!WriteFile(file->hdl, buf, size, &writ_count, nullptr)){
		return AX_INV_BUF;
	}

	file->offset += writ_count;
	if (writ != nullptr){
		*writ = writ_count;
	}

	return AX_SUCC;
}

axres io_fmmap(
	_in void 		*file_hdl,
	_in bool		term,
	_in_out io_fmap		*map
){
	if (file_hdl == nullptr){
		return AX_INV_ARG;
	}
	if (map == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	LARGE_INTEGER map_size = {0};
	void *map_root = nullptr;
	HANDLE map_hdl = INVALID_HANDLE_VALUE;

	// Get file size
	if (!GetFileSizeEx(file_hdl, &map_size)){
		return AX_INV_DATA;
	}

 	map_size.QuadPart += ((term == false) ? sizeof(u64) : 0);
	/*
	 	Create map for the file
	*/
	map_hdl = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		nullptr,
		PAGE_READWRITE,
		map_size.HighPart,
		map_size.LowPart,
		nullptr
	);
	if (map_hdl == INVALID_HANDLE_VALUE){
		res = AX_INV_MEM;
		goto error_jump;
	}
 	map_size.QuadPart -= ((term == false) ? sizeof(u64) : 0);

	/*
	 	Allocate buffer for the file
	*/
	map_root = MapViewOfFile(
		map_hdl,
		FILE_MAP_READ | FILE_MAP_WRITE,
		0,
		0,
		map_size.QuadPart
	);
	if (map_root == nullptr){
		res = AX_INV_MEM;
		goto error_jump;
	}

	/*
		Read to file map
	*/
	if (!ReadFile(
		file_hdl,
		map_root,
		map_size.QuadPart,
		nullptr,
		nullptr)
	){
		res = AX_INV_FILE;
		goto error_jump;
	}

	map->hdl = map_hdl;
	map->root = map_root;
	map->size = map_size.QuadPart;

	return AX_SUCC;

error_jump:
	UnmapViewOfFile(map_root);
	CloseHandle(map_hdl);

	return res;
}
void io_funmap(
	_in_out io_fmap		*map
){
	if (map == nullptr){
		return;
	}

	CloseHandle(map->hdl);
	UnmapViewOfFile(map->root);
	map->size = 0;
}

#elif defined(AX_KM)
#endif // defined(AX_UM) 

#endif

