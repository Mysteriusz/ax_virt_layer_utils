#if !defined(AX_IO_MEMORY)
#define AX_IO_MEMORY

#include "ax_type.h"

/*
 	These macros should be used when writing multi-platform/multi-build memory abstraction.
*/

#if defined(AX_UM)

#if defined(AX_WIN64)

#define axmalloc(size) ({ \
	void *ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size)); \
	asrt(ptr != nullptr); \
	ptr; \
})
#define axfree(ptr) ({ \
	if (ptr) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr)); \
})
#define axmmap() ({ \
})

#elif defined(AX_LINUX)

#error "MAKE ASSERTIONS"
#define axmalloc(size) malloc((size))
#define axfree(ptr) free((ptr))

#endif // defined(AX_WIN64)

#elif defined(AX_KM)

#if defined(AX_WIN64)

#define AX_MEM_POOL_TAG 		'axkm'
static void *axmalloc_msvc(
	_in u64			size
){
	void *ptr = ExAllocatePool2(NonPagedPoolNx, size, AX_MEM_POOL_TAG);
	asrt(ptr != nullptr);
	return ptr;
}
static void axfree_msvc(
	_in void		*ptr
){
	if (ptr){
		ExFreePool2(ptr, AX_MEM_POOL_TAG, nullptr, 0);
	}
}

#define axmalloc(size) axmalloc_msvc(size)
#define axfree(ptr) axfree_msvc(ptr)

#elif defined(AX_LINUX)

#error "MAKE ASSERTIONS"
#define axmalloc(size) kmalloc((size), GFP_KERNEL) 
#define axfree(ptr) free((ptr))

#endif // defined(AX_WIN64)

#endif // defined(AX_UM)

i8 sfmemcmp(
	_in const void* a,
	_in const void* b,
	_in const u64 a_n,
	_in const u64 b_n
);

#endif // !defined(AX_IO_MEMORY)

