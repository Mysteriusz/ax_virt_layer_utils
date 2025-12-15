#include "ax_log.h"

axres io_str(
	_in const c16 		*val
){
#if defined(AX_UM)
	printf("%ls\n", val);
#elif defined(AX_KM)
	DbgPrint("%ws\n", val);
#endif
	return AX_SUCC;
}
axres io_str_b(
	_in const c16 		*val
){
#if defined(AX_UM)
	for (u32 i = 0; i < wcslen(val); i++){
		u8 *val_b = (u8*)&val[i];
		for (u32 j = 0; j < sizeof(c16); j++){
			printf("%02X ", val_b[j]);
		}
	}
	printf("\n");
#elif defined(AX_KM)
	for (u32 i = 0; i < wcslen(val); i++){
		u8 *val_b = (u8*)&val[i];
		for (u32 j = 0; j < sizeof(c16); j++){
			DbgPrint("%02X ", val_b[j]);
		}
	}
	DbgPrint("\n");
#endif
	return AX_SUCC;
}
axres io_str_len(
	_in const c16 		*val,
	_in u64 		len
){
#if defined(AX_UM)
	printf("%.*ls\n", (int)len, val);
#elif defined(AX_KM)
	io_mpkm_log(val);
	unref(val);
#endif

	return AX_SUCC;
}
axres io_i64(
	_in i64			val
){
#if defined(AX_UM)
	printf("%lld\n", val);
#elif defined(AX_KM)
	DbgPrint("%llu\n", val);
#endif
	return AX_SUCC;
}

