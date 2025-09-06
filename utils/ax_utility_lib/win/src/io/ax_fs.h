#if !defined(AX_IO_FS_INT)
#define AX_IO_FS_INT

#include "ax_type.h"
#include "ax_error_code.h"

#define IO_FILE_CHUNK 0x400

typedef u16 io_file_acc;
/*
 	IMPORTANT!

	When you add new flag REMEMBER to upadate io_file_conv
*/
enum io_file_acc{
	IO_FILE_R = 0x01,
	IO_FILE_W = 0x02,
	IO_FILE_C = 0x04,
	IO_FILE_RW = IO_FILE_R | IO_FILE_W,
	IO_FILE_RWC = IO_FILE_R | IO_FILE_W | IO_FILE_C,
};

#define UTF16LE_BOM 	0xFFFE
#define UTF16BE_BOM	0xFEFF
#define UTF32LE_BOM	0xFFFE0000
#define UTF32BE_BOM	0x0000FEFF
#define UTF8_BOM	0xEFBBBF
typedef u32 io_file_enc;
enum io_file_enc{
	UTF16LE = 0, // UTF-16LE is set as default encoding if not provided 
	UTF16BE = 1,
	UTF32LE = 2,
	UTF32BE = 3,
	UTF8 = 4,
};

static inline const c16 *io_file_conv(
	_in io_file_acc 		acc	
){
	switch(acc){
	case IO_FILE_R:
		return L"rb";
	case IO_FILE_W:
		return L"ab";
	case IO_FILE_RW:
		return L"ab+";
	case IO_FILE_C:
		return L"wb";
	case IO_FILE_RWC:
		return L"w+b";
	default:
		return nullptr;
	}
}

typedef struct _io_file{
	c16			*path;
	io_file_acc		acc;
	u64 			size;
#if defined(AX_UM)
	FILE			*hdl;
#elif defined(AX_KM)

#if defined(AX_WIN32)
	HANDLE			hdl;
#elif defined(AX_LINUX)
	void			*hdl;
#endif

#endif
	io_file_enc 		enc; // Optional field (default = UTF16LE)
	u64			offset; // Optional field (default = 0) ** RESETS AFTER R/W OPERATIONS **
} io_file;
bool io_file_inv(
	_in io_file 		*file
);

// Get file byte order mark (Encoding)
axres io_fbom(
	_in const c16		*path,
	_out io_file_enc	*buf
);

// Check file existance 
axres io_fex(
	_in const c16		*path	
);

// Open file 
axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_out io_file		*buf
);
// Close file 
axres io_fc(
	_in io_file		*file
);
// Read file
axres io_fr(
	_in io_file		*file,
	_in u64			size,
	_in_out void 		*buf,
	_out_opt u64		*read // Bytes read
);
// Write file
axres io_fw(
	_in io_file		*file,
	_in u64			size,
	_in void 		*buf,
	_out_opt u64		*writ // Bytes written
);

#endif // AX_IO_FS_INT

