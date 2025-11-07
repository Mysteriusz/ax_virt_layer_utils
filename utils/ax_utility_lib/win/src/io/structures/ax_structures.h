#if !defined(AX_STRUCTURES_INT)
#define AX_STRUCTURES_INT

#include "ax_error.h"
#include "ax_memory.h"

enum ax_structure_type{
	structure_seq = 1, // Sequential structure type
	structure_asc = 2 // Associative structure type
};
#define index_as(s, v_t, ...) ((v_t)s->query_at(s, __VA_ARGS__))

/*

 	Sequential data structures interface

*/

typedef axres (*ax_structures_add)(
	_in void 			*structure,
	_in void 			*value,
	_in u64 			size
);

typedef axres (*ax_structures_remove)(
	_in void 			*structure,
	_in void 			*value,
	_in u64 			size
);

typedef axres (*ax_structures_contains)(
	_in void 			*structure,
	_in void 			*value,
	_in u64 			size
);

typedef axres (*ax_structures_at)(
	_in void 			*structure,
	_in u32 			index,
	_out const void 		**structure_node
);
typedef const void *(*ax_structures_query_at)(
	_in void 			*structure,
	_in u32 			index
);

typedef axres (*ax_structures_clear)(
	_in void 			*structure
);

typedef axres (*ax_structures_delete)(
	_in void 			*structure
);

// There is always one iterator per-structure
typedef u8 _iter_code; 
typedef enum _iter_code{
	ITER_NONE = 0x00, // Continue as normal
	ITER_STOP = 0x01, // Stop iterator
	ITER_REDO = 0x02, // Redo action
} iter_code;
typedef iter_code (*ax_iter_act)(
	// Stack is structure dependant but it always contains field for user-defined data
	const u8 			stack _prepass
);
typedef iter_code (*ax_structures_iter)(
	_in void 			*structure,
	_in ax_iter_act			action,
	_in_opt void 			*data,
	_out const void			**structure_node
);

// Command set for linear (sequential) data structures
#define AX_STRUCTURE_CMD_SEQ \
	ax_structures_add add; \
	ax_structures_remove remove; \
	ax_structures_contains contains; \
	ax_structures_at at; \
	ax_structures_query_at query_at; \
	ax_structures_clear clear; \
	ax_structures_delete delete; \
	ax_structures_iter iter;

/*

 	Associative data structures interface

*/

typedef axres (*ax_structures_add_kv)(
	_in void 			*structure,
	_in void 			*key,
	_in u64 			key_size,
	_in void 			*value,
	_in u64 			value_size
);
typedef axres (*ax_structures_contains_k)(
	_in void 			*structure,
	_in void 			*key,
	_in u64 			key_size
);
typedef axres (*ax_structures_remove_kv)(
	_in void 			*structure,
	_in void 			*value,
	_in u64 			value_size
);
typedef axres (*ax_structures_at_k)(
	_in void 			*structure,
	_in void			*key,
	_in u64 			key_size,
	_out const void 		**structure_node
);
typedef const void *(*ax_structures_query_at_k)(
	_in void 			*structure,
	_in void 			*key,
	_in u64 			key_size
);

// Command set for associative (key-value pair) data structures
#define AX_STRUCTURE_CMD_ASC \
	ax_structures_add_kv add; \
	ax_structures_remove_kv remove; \
	ax_structures_contains_k contains_key; \
	ax_structures_at_k at; \
	ax_structures_query_at_k query_at; \
	ax_structures_clear clear; \
	ax_structures_delete delete;

#endif

