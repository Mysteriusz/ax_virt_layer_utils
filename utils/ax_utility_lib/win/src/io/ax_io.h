#if !defined(AX_IO_INT)
#define AX_IO_INT

#include "ax_type.h"
#include "ax_error_code.h"

#if defined(AX_KM)

#if defined(AX_WIN32)
#define io_mpkm_log(val) KdPrint((val))
#elif defined(AX_LINUX)
// TODO: linux abstraction (printk??) 
#endif

#endif

axres io_str(
	const c16 		*val
);
axres io_i64(
	i64			val
);

// I/O headers 
#include "ax_mem.h"
#include "ax_fs.h"

#endif // !defined(AX_IO_INT)

