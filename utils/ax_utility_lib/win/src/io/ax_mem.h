#include "ax_type.h"
#include "ax_error_code.h"

#if defined(AX_UM)

#define axmalloc(size) malloc((size))
#define axfree(ptr) free((ptr))

#elif defined(AX_KM)

#if defined(AX_WIN32)

#define AX_MEM_POOL_TAG 		'axkm'
#define axmalloc(size) ExAllocatePool2(NonPagedPoolNx, (size), AX_MEM_POOL_TAG)
#define axfree(ptr) ExFreePool2((ptr), AX_MEM_POOL_TAG, nullptr, 0)

#elif defined(AX_LINUX)

#define axmalloc(size) kmalloc((size), GFP_KERNEL) 
#define axfree(ptr) free((ptr))

#endif

#endif

