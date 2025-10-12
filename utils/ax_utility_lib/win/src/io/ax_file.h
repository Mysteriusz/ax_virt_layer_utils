#if !defined(AX_IO_FS_INT)
#define AX_IO_FS_INT

#include "ax_type.h"
#include "ax_error_code.h"

#define IO_FILE_CHUNK 0x400 // (1024 UTF8) (512 UTF16) (256 UTF32)

typedef u16 io_file_acc;
/*
 	IMPORTANT!

	When adding a new flag REMEMBER to upadate io_file_conv
*/
enum io_file_acc{
	// Read
	IO_FILE_R = 0x01,
	// Read/Write(Append)/Create
	IO_FILE_W = 0x02,
	// Read/Write/Create(Truncate)
	IO_FILE_C = 0x04,
	IO_FILE_RW = IO_FILE_R | IO_FILE_W,
	IO_FILE_RWC = IO_FILE_R | IO_FILE_W | IO_FILE_C,
};

#define UTF16LE_BOM 	0xFEFF
#define UTF16BE_BOM	0xFFFE
#define UTF32LE_BOM	0xFEFF0000
#define UTF32BE_BOM	0x0000FFFE
#define UTF8LE_BOM	0xBFBBEF
#define UTF8BE_BOM	0xEFBBBF
typedef u32 io_file_enc;
enum io_file_enc{
	UTF8LE = 10,
	UTF8 = 11,
	UTF8BE = 12,

	UTF16LE = 20,	
	UTF16 = 21, 
	UTF16BE = 22,

	UTF32LE = 40,
	UTF32 = 41,
	UTF32BE = 42,
};
// Bytes size of the file encoding
static inline u32 _enc_size(
	_in io_file_enc 		enc
){
	switch(enc){
	case UTF8LE:
	case UTF8:
	case UTF8BE:
		return 1;

	case UTF16LE:
	case UTF16:
	case UTF16BE:
		return 2;

	case UTF32LE:
	case UTF32:
	case UTF32BE:
		return 4;
	default:
		return 0;
	}
}

static inline const c16 *_io_file_conv(
	_in io_file_acc 		acc	
){
	switch(acc){
	case IO_FILE_R:
		return L"rb";
	case IO_FILE_W:
		return L"ab";
	case IO_FILE_C:
		return L"wb";
	case IO_FILE_RW:
		return L"ab+";
	case IO_FILE_RWC:
		return L"wb+";
	default:
		return nullptr;
	}
}

typedef struct _io_fmap{
	void *root; 

#if defined(AX_WIN64)
	HANDLE hdl;
#elif defined(AX_LINUX)
#error "TODO"
#endif // defined(AX_WIN64)

} io_fmap;

typedef struct _io_file{
	_heap c16		*path;
#if defined(AX_UM)
	FILE			*hdl;
#elif defined(AX_KM)

#if defined(AX_WIN64)
	HANDLE			hdl;
#elif defined(AX_LINUX)
	void			*hdl;
#endif

#endif
	io_fmap			map; // Memory mapped file
	io_file_acc		acc; // File access
	io_file_enc 		enc; // Optional field (default = UTF16LE)
	u64			offset; // (IN BYTES) Optional field (default = 0)
} io_file;

// Invalidate file and check encoding (exp_enc = 0 if no encoding check)
bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_enc 	exp_enc // Pass expected encoding
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

// Check file size
axres io_fsize(
	_in const c16		*path,
	_out u64		*size
);

// Open BOM encoded file
axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_out io_file		**buf
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

axres io_fmmap(
	_in void 		*hdl,
	_in_out io_fmap		*map
);
axres io_funmap(
	_in_out io_fmap		*map
);

#endif // AX_IO_FS_INT

