#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "ax_type.h"
#include "ax_error_code.h"

// Error structure cast
#define AX_RES_S(r) 			(*(axres_s*)((axres*)addr(r)))

// Error code check
inline bool AX_ERR(axres res){
	axres_s res_s = AX_RES_S(res);

	if (res_s.err != (axres_s){AX_SUCC}.err){
		DbgPrint("AXRES ERROR CODE: %llu\n", res_s.err);
		return true;
	}

#if defined(AX_WIN64) && defined(AX_KM)
	// NTSTATUS type error code (WIN64 KM specific)
	if(res_s.meta.ntstatus == true
	&& NT_ERROR(res_s.meta_err)){
		DbgPrint("NTSTATUS ERROR CODE: %llu\n", res_s.meta_err);
		return true;	
	}
#endif

	return false;
};

/*
 	Inline return AX_ERR(r) alias
*/
#define axcheck(r,...)			do { \
	if(AX_ERR(r)){__VA_ARGS__; return ((axres)r);} \
} while(0)

/*
 	Inline custom value return AX_ERR(r) alias
*/
#define axcheck_r(r, fr,...)		do { \
	if(AX_ERR(r)){__VA_ARGS__; return (fr);} \
} while(0)

/*
 	Inline goto AX_ERR(r) alias
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

static void ax_log_msg(
	axres 			res,
	const c16		*msg
){
	ax_log(res);
	io_str(u"Message buffer:");
	io_str(msg);
}

#endif // !defined(AX_ERROR_INT)

