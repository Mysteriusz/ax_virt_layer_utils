#if !defined(AX_ERROR_CODE_INT)
#define AX_ERROR_CODE_INT

/*
 
   	Code mapping:

   	axres bit 0-11 		: Error code
   	axres bit 12-15		: Error metadata

	Further bit fields may be added
   	
	Code structure can be revealed by casting 
		- (axres_s*)&axres

*/

#include "ax_type.h"

// ERROR CODES CANNOT BE BIGGER THAN 0xFFF (12 bits) 
#define AX_SUCC 			((axres)0x00)

// "INVALID" codes

#define AX_INV_ARG 			((axres)0x01)
#define AX_INV_ARG_MSG 			L"Invalid argument passed."

#define AX_INV_DATA 			((axres)0x02)
#define AX_INV_DATA_MSG 		L"Invalid data passed."

#define AX_INV_BUF 			((axres)0x03)
#define AX_INV_BUF_MSG 			L"Invalid buffer passed."

#define AX_INV_CODE 			((axres)0x04)
#define AX_INV_CODE_MSG 		L"Invalid code received."

#define AX_INV_FILE 			((axres)0x05) // EXCLUSIVE TO _io_file STRUCTURE ERRORS
#define AX_INV_FILE_MSG 		L"Invalid file structure."

#define AX_INV_ENC 			((axres)0x06) // EXCLUSIVE TO _io_file_enc TYPE ERRORS
#define AX_INV_ENC_MSG 			L"Invalid file encoding."

#define AX_INV_FMT 			((axres)0x07)
#define AX_INV_FMT_MSG 			L"Invalid value format."

#define AX_INV_IND 			((axres)0x08)
#define AX_INV_IND_MSG 			L"Index out of bounds."

// "BUFFER" codes

#define AX_BUF_TOO_SMALL 		((axres)0x10)
#define AX_BUF_TOO_SMALL_MSG 		L"Buffer too small."

#define AX_BUF_TOO_BIG 			((axres)0x11)
#define AX_BUF_TOO_BIG_MSG 		L"Buffer too big."

// "NOT" codes

#define AX_NOT_FND 			((axres)0x20)
#define AX_NOT_IMP 			((axres)0x21)

// "ACCESS" codes

#define AX_ACC_DEN			((axres)0x40)

// "UNKNOWN" codes

#define AX_UNK_ERR 			((axres)0x80)

static inline axres _ax_buf_err(
	u64 		size,
	u64 		buf_size	
){
#if defined(AX_STRICT_BUF_SIZE)
	if (size < buf_size){
		return AX_BUF_TOO_BIG;
	}
#endif
	if (size > buf_size){
		return AX_BUF_TOO_SMALL;
	}

	return AX_SUCC;
}

#endif // !defined(AX_ERROR_CODE_INT)

