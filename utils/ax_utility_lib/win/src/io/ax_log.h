#include "ax_type.h"
#include "ax_error_code.h"

#if defined(AX_KM)

#if defined(AX_WIN64)
#define io_mpkm_log(val) KdPrint((val))
#elif defined(AX_LINUX)
// TODO: linux abstraction (printk??) 
#endif

#endif

axres io_str(
	_in const c16 		*val
);
axres io_str_b(
	_in const c16 		*val
);
axres io_str_len(
	_in const c16 		*val,
	_in u64 		len
);
axres io_i64(
	_in i64			val
);
