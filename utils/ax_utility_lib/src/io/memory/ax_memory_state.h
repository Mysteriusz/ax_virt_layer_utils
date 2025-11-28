#include "ax_type.h"

/*
 	Global memory state variables.
*/

// Memory logging enabled/disabled
extern bool _MEM_LOG;

// All allocated
extern u64 _MEM_USED;

// Allocated not freed
extern u64 _MEM_ACTIVE;

// Freed after allocating
extern u64 _MEM_FREED;

