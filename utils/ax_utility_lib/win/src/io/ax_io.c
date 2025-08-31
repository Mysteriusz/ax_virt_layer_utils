#include "ax_io.h"

axres io_str(
	const c16 		*val
){
	printf("%ls\n", val);
	return AX_SUCC;
}
axres io_i64(
	i64			val
){
	printf("%lld\n", val);
	return AX_SUCC;
}

