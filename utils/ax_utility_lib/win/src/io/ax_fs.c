#include "ax_fs.h"
#include "ax_error.h"

axres io_fex(
	_in c16			*path	
){
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
// TODO
#endif
	return AX_SUCC;
}

axres io_fo(
	_in c16			*path,
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
		return err;
	}

	// Get size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

#elif defined(AX_KM)

#if defined(AX_WIN32)
	// TODO: 
#elif defined(AX_LINUX)
	// TODO: 
#endif

#endif

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
		return AX_INV_ARG;
	}

#if defined(AX_UM)

	errno_t err = 0;
	err = fclose(file->hdl);
	if (err != 0){
		return err;
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
	_in_out void 		*buf
){
	if (file == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	if (size > file->size){
		return AX_BUF_TOO_BIG;
	}

#if defined(AX_UM)

	errno_t err = 0;
	err = fread(buf, size, 1, file->hdl);	
	if (err != 0){
		return AX_INV_DATA;
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
	_in void 		*buf
){
	if (file == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

#if defined(AX_UM)

	u64 writ = fwrite(buf, size, 1, file->hdl);	
	if (writ == 0){
		return AX_INV_DATA;
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

