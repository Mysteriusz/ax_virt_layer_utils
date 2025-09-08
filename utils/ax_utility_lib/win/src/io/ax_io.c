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
axres io_str_b(
	const c16 		*val
){
#if defined(AX_UM)
	for (u32 i = 0; i < wcslen(val); i++){
		u8 *val_b = (u8*)&val[i];
		for (u32 j = 0; j < sizeof(c16); j++){
			printf("%02X", val_b[j]);
		}
		printf(" ");
	}
	printf("\n");
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

