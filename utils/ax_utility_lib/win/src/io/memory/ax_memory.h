#if !defined(AX_IO_MEMORY)
#define AX_IO_MEMORY

#include "ax_type.h"

/*
 	These macros should be used when writing multi-platform/multi-build memory abstraction.
*/

#if defined(AX_WIN64)

#include "ax_memory_win64.h"

#elif defined(AX_LINUX)

#include "ax_memory_linux.h"

#endif // defined(AX_WIN64)

#define axmalloc(size) CAT(CAT(axmalloc_,AX_PLATFORM),CAT(_,AX_MODE))(size, __LINE__, (c8*)__FUNCTION__)
#define axfree(size) CAT(CAT(axfree_,AX_PLATFORM),CAT(_,AX_MODE))(size, __LINE__, (c8*)__FUNCTION__)

static i8 _sfmemcmp(
	_in const u8 a _prepass,
	_in const u8 b _prepass,
	_in u32 a_n,
	_in u32 b_n
){
	if (a_n != b_n){
		return a_n < b_n ? -1 : 1;
	}

	asrt(a != nullptr);
	asrt(b != nullptr);

	i32 i = a_n - 1;

	while(i >= 0){
		if (a[i] > b[i]){
			return -1;
		}else if (a[i] < b[i]){
			return 1;
		}
		i--;
	}

	return 0;
}

#endif // !defined(AX_IO_MEMORY)

