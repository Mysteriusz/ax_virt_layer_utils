#include "ax_memory.h"

u64 _MEM_USED;
u64 _MEM_ACTIVE;
u64 _MEM_FREED;

i8 sfmemcmp(
	_in const void* a,
	_in const void* b,
	_in const u64 a_n,
	_in const u64 b_n
){
	if (a_n != b_n){
		return a_n < b_n ? -1 : 1;
	}
	return memcmp(a, b, a_n);
}

