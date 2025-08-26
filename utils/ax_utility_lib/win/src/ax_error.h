#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "ax_type.h"

/*
 
   	axres >= 1 -> result is an error in user-mode
   	axres <= -1 -> result is an error in kernel-mode
   	
*/

#define AX_SUCC 			((axres)0x00)

#define AX_INV_ARG 			((axres)0x01)
#define AX_INV_DATA 			((axres)0x02)
#define AX_INV_BUF 			((axres)0x03)

#define AX_BUF_TOO_SMALL 		((axres)0x10)
#define AX_BUF_TOO_BIG 			((axres)0x11)

#define AX_NOT_FND 			((axres)0x20)
#define AX_NOT_IMP 			((axres)0x21)

#define AX_ACC_DEN			((axres)0x40)

#define AX_ERR(r) 			(r != AX_SUCC)
#define AX_KMERR(r) 			(r < AX_SUCC)
#define AX_UMERR(r) 			(r > AX_SUCC)

#define AX_LOG_HEAD 			L"--------AX_LOG--------"

static void ax_log(
	axres 			res
){
	_putws(AX_LOG_HEAD);
	printf("%lld\n", res);
}

static void ax_log_msg(
	axres 			res,
	const c16		*msg
){
	ax_log(res);
	_putws(L"Message buffer:");
	_putws(msg);
}

#if defined(AX_WIN32)

#define AX_LOG_HEAD_LSTAT 		L"--------LSTATUS--------"

static void ax_log_lstat(
	LSTATUS 		stat
){
	ax_log(stat);
	_putws(AX_LOG_HEAD_LSTAT);
}

#endif // defined(AX_WIN32)

#endif // !defined(AX_ERROR_INT)

