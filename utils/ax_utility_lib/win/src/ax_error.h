#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "ax_type.h"
#include "ax_error_code.h"

// Error structure cast
#define AX_RES_S(r) 			(*(axres_s*)((axres*)addr(r)))

// Error code check
#define AX_ERR(r) 			((AX_RES_S(r).err) != AX_SUCC)

/*
 	Inline return AX_ERR(r) alias
*/
#define axcheck(r,...)			do { \
	if(AX_ERR(r)){__VA_ARGS__; return ((axres)r);} \
} while(0)

/*
 	Inline custom return AX_ERR(r) alias
*/
#define axcheck_r(r, fr,...)		do { \
	if(AX_ERR(r)){__VA_ARGS__; return (fr);} \
} while(0)

/*
 	Inline custom goto AX_ERR(r) alias
*/
#define axcheck_g(r, gt, ...)		do { \
	if(AX_ERR(r)){__VA_ARGS__; goto gt;} \
} while(0)

/*
 	Inline break AX_ERR(r) alias
*/
#define axcheck_b(r,...)		  ({ \
	if(AX_ERR(r)){__VA_ARGS__; break;}  \
})

#define AX_LOG_HEAD 			u"--------AX_LOG--------"
#define AX_LOG_META 			u"--------METADATA--------"

#include "ax_log.h"

static void ax_log(
	axres 			res
){
	io_str(AX_LOG_HEAD);
	io_i64(AX_RES_S(res).err);
}

#if defined(AX_WIN64)

static void ax_log_lstat(
	axres 			stat
){
	ax_log(stat);
	io_str(u"--------LSTATUS--------");
}

#endif // defined(AX_WIN64)

static void ax_log_msg(
	axres 			res,
	const c16		*msg
){
	ax_log(res);
	io_str(u"Message buffer:");
	io_str(msg);
}

#endif // !defined(AX_ERROR_INT)

