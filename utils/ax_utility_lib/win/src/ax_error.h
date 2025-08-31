#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "ax_type.h"
#include "ax_error_code.h"

#define AX_ERR(r) 			(r != AX_SUCC)
#define AX_KMERR(r) 			(r < AX_SUCC)
#define AX_UMERR(r) 			(r > AX_SUCC)

#define AX_LOG_HEAD 			L"--------AX_LOG--------"

#include "ax_io.h"

static void ax_log(
	axres 			res
){
	io_str(AX_LOG_HEAD);
	io_i64(res);
}

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

#if defined(AX_WIN32)

static void ax_log_lstat(
	axres 			stat
){
	ax_log(stat);
	io_str(L"--------LSTATUS--------");
}

#endif // defined(AX_WIN32)

static void ax_log_msg(
	axres 			res,
	const c16		*msg
){
	ax_log(res);
	io_str(L"Message buffer:");
	io_str(msg);
}

#endif // !defined(AX_ERROR_INT)

