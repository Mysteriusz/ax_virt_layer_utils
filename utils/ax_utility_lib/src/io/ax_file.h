#if !defined(AX_IO_FS_INT)
#define AX_IO_FS_INT

#include "ax_type.h"
#include "ax_memory.h"
#include "ax_error_code.h"

/*
	All file operations with exception of path queried ones,
	are currently limited to 0x80000000 (2 gigibytes) size.
*/
#define IO_FILE_MAX GB(2) // Maximum file size

#define IO_FILE_CHUNK 0x400 // (1024 UTF8) (512 UTF16) (256 UTF32)

// File access bit mask
typedef u16 io_file_acc;
/*
 	IMPORTANT!

	When adding a new flag REMEMBER to upadate io_file_conv
*/
enum io_file_acc{
	// Read
	IO_FILE_R = 0x01,
	// Write
	IO_FILE_W = 0x02,
	// Create(If doesnt exist)
	IO_FILE_C = 0x04,
	IO_FILE_RW = IO_FILE_R | IO_FILE_W,
	IO_FILE_RWC = IO_FILE_R | IO_FILE_W | IO_FILE_C,
};

// File information bit mask
typedef u16 io_file_inf;
enum io_file_inf{
	IO_FILE_MAP = 0x01,
	IO_FILE_ENC = 0x02,
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
// Byte size of the file encoding
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
// Byte count of the bom
static inline u32 _bom_size(
	_in io_file_enc 		bom
){
	switch(bom){
	case UTF8LE:
	case UTF8:
	case UTF8BE:
		return 3;
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

#if defined(AX_WIN64)
static inline const DWORD _io_file_conv_win64(
	_in io_file_acc 		acc	
){
	switch(acc){
	case IO_FILE_R:
		return FILE_READ_DATA;
	case IO_FILE_W:
		return FILE_WRITE_DATA;
	case IO_FILE_C:
		return FILE_ADD_FILE;
	case IO_FILE_RW:
		return FILE_READ_DATA | FILE_WRITE_DATA;
	case IO_FILE_RWC:
		return FILE_READ_DATA | FILE_WRITE_DATA | FILE_ADD_FILE;
	default:
		return 0;
	}
}
#endif // defined(AX_WIN64)

static inline const c16 *_io_file_conv(
	_in io_file_acc 		acc	
){
	switch(acc){
	case IO_FILE_R:
		return u"rb";
	case IO_FILE_W:
		return u"ab";
	case IO_FILE_C:
		return u"wb";
	case IO_FILE_RW:
		return u"a+b";
	case IO_FILE_RWC:
		return u"w+b";
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

	u32 size;
} io_fmap;

typedef struct _io_file{
	c16 *path;
#if defined(AX_UM)
	void *hdl;
#elif defined(AX_KM)

#if defined(AX_WIN64)
	HANDLE hdl;
#elif defined(AX_LINUX)
	void *hdl;
#endif

#endif
	io_file_acc acc; // File access bit mask
	io_file_inf inf; // File info bit mask
	io_fmap	map; // Memory mapped file
	io_file_enc enc; // Optional field (default = UTF16LE)
	u64 offset; // (IN BYTES) Changes after each R/W (default = _bom_size(enc))
} io_file;

// Invalidate file
bool io_finv(
	_in io_file 		*file,
	_in_opt io_file_inf 	exp_inf, // Pass expected file resources
	_in_opt io_file_enc 	exp_enc // Pass expected encoding
);

// Set file offset
bool io_foff(
	_in io_file 		*file,
	_in u32 		offset
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

// Create/Open BOM encoded file
axres io_fo(
	_in const c16		*path,
	_in io_file_acc		acc,
	_in io_file_inf		inf,
	_out io_file		**buf
);
// Create/Open TEMP file
axres io_fo_tmp(
	_out io_file		**buf
);
// Force close file 
void io_fc(
	_in io_file		*file
);
// Read file
axres io_fr(
	_in io_file		*file,
	_in u32			size,
	_in_out void 		*buf,
	_out_opt u32		*read // Bytes read
);
// Write file
axres io_fw(
	_in io_file		*file,
	_in u32			size,
	_in void 		*buf,
	_out_opt u32		*writ // Bytes written
);

/*
	Transfer file content between files
	CAUTION:
		Data will be overwritten if pointers are not set to EOF
	
	size == 0 will transfer the entire file contents
*/
axres io_ftrans(
	_in io_file		*from,
	_in io_file		*to,
	_in u32			size,
	_out_opt u32		*trans // Bytes transfered
);
// Resize file
axres io_fres(
	_in io_file		*file,
	_in u32			size
);

// Map file to memory
axres io_fmmap(
	_in void 		*hdl,
	_in bool 		term,
	_in_out io_fmap		*map
);
void io_funmap(
	_in_out io_fmap		*map
);

#endif // AX_IO_FS_INT

