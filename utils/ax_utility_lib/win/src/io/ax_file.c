#include "ax_file.h"
#include "ax_error.h"

bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_enc 	exp_enc
){
	if (file == nullptr){
		return true;
	}
	if (file->path == nullptr
	|| AX_ERR(io_fex(file->path))){
		return true;
	}
	if (file->hdl == nullptr){
		return true;
	}

	if (exp_enc != 0
	&& _enc_size(file->enc) != _enc_size(exp_enc)){
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

#if defined(AX_UM)
	FILE *file = nullptr;
	errno_t err = _wfopen_s(&file, path, L"r"); 
	if (err != 0){
		if (err == ENOENT){
			return AX_NOT_FND;
		}
		return AX_INV_FILE;
	}
	
	u32 *bom = axmalloc(4); // Max BOM size is 4 bytes
	u64 read = fread(bom, 1, 4, file);

	io_file_enc enc = 0;

	// Read size CAN be UTF32
	if (read == 4){
		if(memcmp(bom, addr(UTF32LE_BOM), 4) == 0){
			enc = UTF32LE;
		}else if(memcmp(bom, addr(UTF32BE_BOM), 4) == 0){
			enc = UTF32BE;
		}
	}
	// Read size CAN be UTF8
	if (read >= 3){
		if(memcmp(bom, addr(UTF8LE_BOM), 3) == 0){
			enc = UTF8LE;
		}else if(memcmp(bom, addr(UTF8BE_BOM), 3) == 0){
			enc = UTF8BE;
		}
	}
	// Read size CAN be UTF16
	if (read >= 2){
		if(memcmp(bom, addr(UTF16LE_BOM), 2) == 0){
			enc = UTF16LE;
		}else if(memcmp(bom, addr(UTF16BE_BOM), 2) == 0){
			enc = UTF16BE;
		}
	}

	if (enc == 0){
		return AX_INV_ENC;
	}

	*buf = enc;

	axfree(bom);
	fclose(file);

#elif defined(AX_KM)

#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	return AX_SUCC;
}

axres io_fex(
	_in const c16		*path	
){
	if (path == nullptr){
		return AX_INV_ARG;
	}

#if defined(AX_UM)
	FILE *file = nullptr;
	errno_t err = _wfopen_s(&file, path, L"r"); 
	if (err != 0){
		if (err == ENOENT){
			return AX_NOT_FND;
		}
		return AX_INV_DATA;
	}
	fclose(file);

#elif defined(AX_KM)

#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	return AX_SUCC;
}

axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_out io_file		*buf
){
	if (path == nullptr
	|| acc == 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	void *file = nullptr;
	u64 size = 0;
#if defined(AX_UM)
	bool exists = (io_fex(path) == AX_SUCC);
	if (!exists
	&& !chkf(acc, IO_FILE_C)){
		return AX_NOT_FND;
	}

	errno_t err = 0;

	// Convert acc to mode
	const c16 *mode = _io_file_conv(acc);
	if (mode == nullptr){
		return AX_INV_CODE;
	}

	// Try to open
	err = _wfopen_s((FILE**)&file, path, mode);
	if (err == ENOENT){
		return AX_NOT_FND;
	} else if (err != 0){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	// Get encoding
	io_file_enc enc = UTF16LE;
	res = io_fbom(path, &enc);
	axcheck(res);

	// Get size
	_fseeki64((FILE*)file, 0, SEEK_END);
	size = _ftelli64((FILE*)file);
	_fseeki64((FILE*)file, 0, SEEK_SET);

#elif defined(AX_KM)

#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	buf->offset = 0;
	buf->path = _wcsdup(path);
	buf->acc = acc;
	buf->enc = enc;
	buf->hdl = file;			
	buf->size = size;

	return AX_SUCC;
}
axres io_fc(
	_in io_file		*file
){
	if (file == nullptr){
		return AX_INV_FILE;
	}

#if defined(AX_UM)

	axfree(file->path);

	errno_t err = 0;
	err = fclose(file->hdl);
	if (err != 0){
		return AX_INV_DATA;
	}

#elif defined(AX_KM)

#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	return AX_SUCC;
}
axres io_fr(
	_in io_file		*file,
	_in u64			size,
	_in_out void 		*buf,
	_out_opt u64		*read
){
	if (file == nullptr){
		return AX_INV_FILE;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}
#if defined(AX_STRICT_BUF_SIZE)
	if (size > file->size){
		return AX_BUF_TOO_BIG;
	}
#endif

#if defined(AX_UM)

	// Skip BOM
	u64 bom_s = _enc_size(file->enc);
	if (file->offset < bom_s){
		file->offset += bom_s - file->offset;
	}

	u64 r = 0;
	_fseeki64(file->hdl, file->offset, SEEK_SET);
	r = fread(buf, 1, size, file->hdl);	

	if (read != nullptr){
		*read = r;
	}

#elif defined(AX_KM)

	unref(read);
#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	return AX_SUCC;
}

axres io_fw(
	_in io_file		*file,
	_in u64			size,
	_in void 		*buf,
	_out_opt u64		*writ
){
	if (file == nullptr){
		return AX_INV_FILE;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

#if defined(AX_UM)

	u64 w = 0;
	_fseeki64(file->hdl, file->offset, SEEK_SET);
	w = fwrite(buf, size, 1, file->hdl);	

	if (writ != nullptr){
		*writ = w;
	}

#elif defined(AX_KM)
#if defined(AX_WIN64)
	#error "TODO"
#elif defined(AX_LINUX)
	#error "TODO"
#endif

#endif

	return AX_SUCC;
}

