#include "ax_type.h"
#include "ax_memory_state.h"

#if defined(AX_UM)

static void *axmalloc_AXWIN64_AXUM(
	_in u64 size,
	_in u32 line,
	_in const c8* func
){
	void *ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (size));
	asrt(ptr != nullptr);
	if (_MEM_LOG){
		printf("%s: %s"
			" AT LINE: %i"
			" WITH PTR: %p"
			" OF SIZE: %llu\n",
			func, "alloc",
			line, 
			ptr, 
			((u64)size)
		);
	}

	_MEM_ACTIVE += size;
	_MEM_USED += size;
	return ptr;
}
static void axfree_AXWIN64_AXUM(
	_in void *ptr,
	_in u32 line,
	_in const c8* func
){
	if (ptr){
		u64 size = HeapSize(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr));
		if (_MEM_LOG){
			printf("%s: %s"
				" AT LINE: %i"
				" WITH PTR: %p"
				" OF SIZE: %llu\n",
				func, "free", 
				line, 
				ptr, 
				((u64)size)
			);
		}

		_MEM_ACTIVE -= size;
		_MEM_FREED += size;
		HeapFree(GetProcessHeap(), HEAP_ZERO_MEMORY, (ptr));
	}
}

#elif defined(AX_KM)

#define MEM_TAG 0x6d6b7861 // Big endian "axkm"

static void *axmalloc_AXWIN64_AXKM(
	_in u64 size,
	_in u32 line,
	_in const c8* func
){
	return nullptr;
}
static void axfree_AXWIN64_AXKM(
	_in void *ptr,
	_in u32 line,
	_in const c8* func
){
	return;
}

#endif // defined(AX_UM)

