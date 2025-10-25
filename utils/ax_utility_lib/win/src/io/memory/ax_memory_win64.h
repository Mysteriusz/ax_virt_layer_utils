#include "ax_type.h"
#include "ax_memory_state.h"

#if defined(AX_UM)

static void *axmalloc_WIN64_UM(
	_in u64 size
){
	void *ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size));
	asrt(ptr != nullptr);
#if defined(AX_MEMORY_LOG)
	printf("%s: %s AT LINE: %i WITH PTR: %p OF SIZE: %llu\n", __FUNCTION__, "alloc", __LINE__, ptr, ((u64)size));
#endif
	_MEM_ACTIVE += size;
	_MEM_USED += size;
	return ptr;
}
static void axfree_WIN64_UM(
	_in void *ptr
){
	if (ptr){
		u64 size = HeapSize(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr));
#if defined(AX_MEMORY_LOG)
		printf("%s: %s AT LINE: %i WITH PTR: %p OF SIZE: %llu\n", __FUNCTION__, "free", __LINE__, ptr, ((u64)size));
#endif
		_MEM_ACTIVE -= size;
		_MEM_FREED += size;
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr));
	}
}

#elif defined(AX_KM)

#error "TODO"

#endif // defined(AX_UM)

