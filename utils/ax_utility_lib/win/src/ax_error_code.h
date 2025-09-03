#if !defined(AX_ERROR_CODE_INT)
#define AX_ERROR_CODE_INT

/*
 
   	axres >= 1 -> result is an error in user-mode
   	axres <= -1 -> result is an error in kernel-mode
   	
*/

#include "ax_type.h"

#define AX_SUCC 			((axres)0x00)

#define AX_INV_ARG 			((axres)0x01)
#define AX_INV_DATA 			((axres)0x02)
#define AX_INV_BUF 			((axres)0x03)
#define AX_INV_CODE 			((axres)0x04)
#define AX_INV_FILE 			((axres)0x05)

#define AX_BUF_TOO_SMALL 		((axres)0x10)
#define AX_BUF_TOO_BIG 			((axres)0x11)

#define AX_NOT_FND 			((axres)0x20)
#define AX_NOT_IMP 			((axres)0x21)

#define AX_ACC_DEN			((axres)0x40)

#define AX_UNK_ERR 			((axres)0x80)

static inline axres _ax_buf_err(
	u32 		size,
	u32 		buf_size	
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

