#include "ax_type.h"

/*
 	Global memory state variables.
*/

extern u64 _MEM_ID;

// All allocated
extern u64 _MEM_USED;

// Allocated not freed
extern u64 _MEM_ACTIVE;

// Freed after allocating
extern u64 _MEM_FREED;

