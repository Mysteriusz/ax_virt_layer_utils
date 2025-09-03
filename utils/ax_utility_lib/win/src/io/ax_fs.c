#include "ax_fs.h"
#include "ax_error.h"

axres io_fex(
	_in const c16		*path	
){
#if defined(AX_UM)
	FILE *file = nullptr;
	errno_t err = _wfopen_s(&file, path, L"r"); 
	if (err != 0){
		if (err == ENOENT){
			return AX_NOT_FND;
		}
		return AX_INV_FILE;
	}
	fclose(file);

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
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

#if defined(AX_UM)
	bool exists = (io_fex(path) == AX_SUCC);
	if (!exists
	&& !chkf(acc, IO_FILE_C)){
		return AX_NOT_FND;
	}

	FILE *file = nullptr;
	errno_t err = 0;
	u64 size = 0;

	const c16 *mode = io_file_conv(acc);
	if (mode == nullptr){
		return AX_INV_CODE;
	}

	// Try to open
	err = _wfopen_s(&file, path, mode);
	if (err == ENOENT){
		return AX_NOT_FND;
	} else if (err != 0){
		return AX_INV_DATA;
	}

	// Get size
	_fseeki64(file, 0, SEEK_END);
	size = _ftelli64(file);
	_fseeki64(file, 0, SEEK_SET);

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
#endif

#endif

	buf->offset = 0;
	buf->path = _wcsdup(path);
	buf->acc = acc;
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

	errno_t err = 0;
	err = fclose(file->hdl);
	if (err != 0){
		return AX_UNK_ERR;
	}

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
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
	u16 bom = 0;
	fread(&bom, sizeof(c16), 1, file->hdl);
	if (bom != 0xfeff
	&& bom != 0xfffe){
		_fseeki64(file->hdl, 0, SEEK_SET);
	}

	u64 r = 0;
	_fseeki64(file->hdl, file->offset, SEEK_CUR);
	r = fread(buf, 1, size, file->hdl);	
	_fseeki64(file->hdl, 0, SEEK_SET);

	if (read != nullptr){
		*read = r;
	}

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
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
	_fseeki64(file->hdl, 0, SEEK_SET);

	if (writ != nullptr){
		*writ = w;
	}

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
#endif

#endif

	return AX_SUCC;
}

