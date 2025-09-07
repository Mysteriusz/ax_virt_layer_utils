#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "ax_type.h"
#include "ax_error_code.h"

#define AX_ERR(r) 			((axres)r != AX_SUCC)
#define AX_KMERR(r) 			((axres)r < AX_SUCC)
#define AX_UMERR(r) 			((axres)r > AX_SUCC)

/*
 	Inline return AX_ERR(r) alias
*/
#define axcheck(r,...)			do { if(AX_ERR(r)){__VA_ARGS__; return (axres)r;} } while(0)

/*
 	Inline break AX_ERR(r) alias
*/
#define axcheck_b(r,...)		do { if(AX_ERR(r)){__VA_ARGS__; break;} } while(0)

#define AX_LOG_HEAD 			L"--------AX_LOG--------"

#include "ax_io.h"

static void ax_log(
	axres 			res
){
	io_str(AX_LOG_HEAD);
	io_i64(res);
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

