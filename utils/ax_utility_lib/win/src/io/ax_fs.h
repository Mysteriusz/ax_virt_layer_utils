#if !defined(AX_IO_FS_INT)
#define AX_IO_FS_INT

#include "ax_type.h"
#include "ax_error_code.h"

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

} io_file;

// Check file existance 
axres io_fex(
	_in c16			*path	
);

// Open file 
axres io_fo(
	_in c16			*path,
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
	_in_out void 		*buf
);
// Write file
axres io_fw(
	_in io_file		*file,
	_in u64			size,
	_in void 		*buf
);

#endif // AX_IO_FS_INT

