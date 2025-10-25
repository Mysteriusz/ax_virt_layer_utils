#if !defined(AX_IO_MEMORY)
#define AX_IO_MEMORY

#include "ax_type.h"

/*
 	These macros should be used when writing multi-platform/multi-build memory abstraction.
*/

#if defined(AX_WIN64)

#include "ax_memory_win64.h"

#elif defined(AX_LINUX)

#include "ax_memory_linux.c"

#endif // defined(AX_WIN64)

#define axmalloc(size) CAT(CAT(axmalloc_,AX_PLATFORM),CAT(_,AX_MODE))(size)
#define axfree(size) CAT(CAT(axfree_,AX_PLATFORM),CAT(_,AX_MODE))(size)

i8 sfmemcmp(
	_in const void* a,
	_in const void* b,
	_in const u64 a_n,
	_in const u64 b_n
);

#endif // !defined(AX_IO_MEMORY)

