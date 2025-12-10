#if defined(AX_WIN64)

#include "ax_file.h"
#include "ax_parser.h"

#if defined(AX_UM)

bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_inf 	exp_inf, // Pass expected file resources
	_in_opt io_file_enc 	exp_enc // Pass expected encoding
){
	if (file == nullptr){
		return true;
	}

	// Check file size
	u64 fsize = 0;
	if (io_fsize(file->path, &fsize) != AX_SUCC
	|| fsize > IO_FILE_MAX){
		return true;
	}

	/* 
	 	Check file info against expected
		If file->inf and exp_inf are both 0 continue
	*/
	if (!(file->inf == 0 && exp_inf == 0)
	&& !chkf(file->inf, exp_inf)){
		return true;
	}

	/*
	 	Read and invalidate encoding if set in file->inf
	*/
	if (!chkf(file->inf, IO_FILE_ENC)){
		// Expecting encoding but IO_FILE_ENC resource not set
		if (exp_enc != 0){
			return true;
		}
		goto skip_enc;
	}

	io_file_enc file_enc = 0;
	if (io_fbom(file->path, &file_enc) != AX_SUCC){
		return true;
	}
	if (exp_enc != 0
	&& _enc_size(file_enc) != _enc_size(exp_enc)){
		return true;
	}

skip_enc:

	/*
	 	TODO
		map invalidation
	*/
	if (!chkf(file->inf, IO_FILE_MAP)){
		goto skip_map;
	}

skip_map:

	return false;
}

bool io_foff(
	_in io_file 		*file,
	_in u32 		offset
){
	if (file == nullptr){
		return false;
	}
	if (io_finv(file, file->inf, file->enc)){
		return false;
	}

	if (chkf(file->inf, IO_FILE_MAP)
	&& offset >= file->map.size){
		return false;
	}
	
	if (!SetFilePointerEx(
		file->hdl,
		(LARGE_INTEGER){.QuadPart = offset},
		nullptr,
		0)
	){
		return false;
	}
	file->offset = offset;

	return true;
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
		FILE_SHARE_READ | FILE_SHARE_WRITE ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
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
		FILE_SHARE_READ | FILE_SHARE_WRITE,
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
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (hdl == INVALID_HANDLE_VALUE){
		io_str(path);
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
	_in io_file_inf		inf,
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
		FILE_SHARE_READ | FILE_SHARE_WRITE,
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
	if (chkf(inf, IO_FILE_ENC)){
		res = io_fbom(path, &file->enc);
		axcheck_g(res, error_jump);
	}

	/*
	 	Allocate the virtual map
	*/
	if (chkf(inf, IO_FILE_MAP)){
		res = io_fmmap(hdl, true, &file->map);
		axcheck_g(res, error_jump);
	}

	/*
		Write-back non-gathered data
	*/
	file->offset += _bom_size(file->enc);
	file->acc = acc;
	file->inf = inf;
	file->hdl = hdl;
	file->path = _c16dup(path);

	if (io_finv(file, file->inf, file->enc)){
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
axres io_fo_tmp(
	_out io_file		**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	/*
	 	Get file path to create
	*/
	c16 *temp_buf = axmalloc(MAX_PATH * sizeof(c16));
	u32 temp_len_n = GetTempPathW(MAX_PATH, temp_buf);
	if (temp_len_n == 0){
		axfree(temp_buf);
		return AX_UNK_ERR;
	}

	if (GetTempFileNameW(temp_buf, u"", 0, temp_buf) == 0){
		axfree(temp_buf);
		return AX_UNK_ERR;
	}

	// Open file as temp
	res = io_fo(temp_buf, IO_FILE_RWC, 0, buf);

	axfree(temp_buf);
	axcheck(res);

	return AX_SUCC;
}
void io_fc(
	_in io_file		*file
){
	if (file == nullptr){
		return;
	}
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
	if (file == nullptr){
		return AX_INV_ARG;
	}
	// Invalidate and set the offset
	if (io_finv(file, file->inf, file->enc)
	|| !io_foff(file, file->offset)){
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
	if (file == nullptr){
		return AX_INV_ARG;
	}
	// Invalidate and set the offset
	if (io_finv(file, file->inf, file->enc)
	|| !io_foff(file, file->offset)){
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

axres io_ftrans(
	_in io_file		*from,
	_in io_file		*to,
	_in u32			size,
	_out_opt u32		*trans // Bytes transfered
){
	if (from == nullptr
	|| to == nullptr){
		return AX_INV_ARG;
	}
	// Invalidate both files
	if (io_finv(from, from->inf, from->enc)
	|| io_finv(to, to->inf, to->enc)){
		return AX_INV_FILE;
	}
	if (!io_foff(from, from->offset)
	|| !io_foff(to, to->offset)){
		return AX_UNK_ERR;
	}

	// Check file access flag
	if (!chkf(from->acc, IO_FILE_R)
	|| !chkf(to->acc, IO_FILE_W)){
		return AX_ACC_DEN;
	}

	/*
	 	Get file size
	*/
	u64 fsize = 0;
	if (size == 0){
		io_fsize(from->path, &fsize);
		size = fsize;
	}

	u8 buf[IO_FILE_CHUNK];
	memset(buf, 0, IO_FILE_CHUNK);

	DWORD read = 0;
	u32 left = size;
	while(left > 0){
		if (!ReadFile(from->hdl, buf, IO_FILE_CHUNK, &read, nullptr)){
			// TODO: Abort and restore backup
			break;
		}
		if (read == 0){
			break;
		}
		if (!WriteFile(to->hdl, buf, read, nullptr, nullptr)){
			// TODO: Abort and restore backup
			break;
		}
		left -= read;
	}
	memset(buf, 0, IO_FILE_CHUNK);

	if (trans != nullptr){
		*trans = size - left;
	}

	return AX_SUCC;
}

axres io_fres(
	_in io_file		*file,
	_in u32			size
){
	if (file == nullptr){
		return AX_INV_ARG;
	}

	if(io_finv(file, file->inf, file->enc)){
		return AX_INV_FILE;
	}

	u32 pre_offset = file->offset;
	// Set file pointer to desired size
	if (!io_foff(file, size)){
		return AX_INV_FILE;
	}

	// Truncate file to pointer
	if (!SetEndOfFile(file->hdl)){
		io_foff(file, pre_offset);
		return AX_INV_FILE;
	}

	// Reset file pointer
	io_foff(file, pre_offset);

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

