#include "ax_type.h"
#include "ax_error_code.h"

//
// These macros should be used in palaces where there is need for multi-platform support AND multi-build support.
// 

#if defined(AX_UM)

#if defined(AX_WIN32)

#define axmalloc(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size));
#define axfree(ptr) HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr))

#elif defined(AX_LINUX)

#define axmalloc(size) malloc((size))
#define axfree(ptr) free((ptr))

#endif // defined(AX_WIN32)

#elif defined(AX_KM)

#if defined(AX_WIN32)

#define AX_MEM_POOL_TAG 		'axkm'
#define axmalloc(size) ExAllocatePool2(NonPagedPoolNx, (size), AX_MEM_POOL_TAG)
#define axfree(ptr) ExFreePool2((ptr), AX_MEM_POOL_TAG, nullptr, 0)

#elif defined(AX_LINUX)

#define axmalloc(size) kmalloc((size), GFP_KERNEL) 
#define axfree(ptr) free((ptr))

#endif // defined(AX_WIN32)

#endif // defined(AX_UM)

