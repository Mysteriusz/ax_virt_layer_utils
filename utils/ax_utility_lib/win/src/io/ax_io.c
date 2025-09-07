#include "ax_io.h"

axres io_str(
	const c16 		*val
){
#if defined(AX_UM)
	printf("%ls\n", val);
#elif defined(AX_KM)
	io_mpkm_log(val);
	unref(val);
#endif
	return AX_SUCC;
}
axres io_i64(
	i64			val
){
#if defined(AX_UM)
	printf("%lld\n", val);
#elif defined(AX_KM)
	io_mpkm_log(val);
	unref(val);
#endif
	return AX_SUCC;
}

