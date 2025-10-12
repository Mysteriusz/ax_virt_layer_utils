#if !defined(AX_IO_STRUCTURES_INT)
#define AX_IO_STRUCTURES_INT

#include "ax_error.h"
#include "ax_memory.h"

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
	_in const void 			*structure,
	_in u32 			index,
	_out const void 		**structure_node
);

// Unsafe at-index value call
typedef void* (*ax_structures_at_v)(
	_in const void 			*structure,
	_in u32 			index
);
#define index_as(s, v_i, v_t) ((v_t)(s->at_v(s, v_i)))

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
	_in const void 			*structure,
	_in ax_iter_act			action,
	_in_opt void 			*data,
	_out const void			**structure_node
);

// Anonymous commnad structure
#define AX_STRUCTURE_CMD \
	ax_structures_add add; \
	ax_structures_remove remove; \
	ax_structures_contains contains; \
	ax_structures_at at; \
	ax_structures_at_v at_v; \
	ax_structures_clear clear; \
	ax_structures_delete delete; \
	ax_structures_iter iter; \

#if !defined(AX_IO_STRUCTURES_LIST_INT)
#define AX_IO_STRUCTURES_LIST_INT

typedef struct _ax_list_node ax_list_node;
typedef struct _ax_list_node{
	_heap void *value;
	u64 size;
	_heap ax_list_node *next;
} ax_list_node;
typedef struct _ax_list{
	_heap ax_list_node *root;
	u32 count;
	struct { AX_STRUCTURE_CMD };
} ax_list;

axres ax_list_init(
	_out ax_list		**buf
);
axres ax_list_add(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
);
axres ax_list_contains(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
);
axres ax_list_remove(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
);
axres ax_list_at(
	_in const ax_list 		*list,
	_in u32 			index,
	_out const ax_list_node 	**buf
);
void *ax_list_at_v(
	_in const ax_list 		*list,
	_in u32 			index
);

typedef struct _ax_list_iter_stack{
	_heap const ax_list 		*list;
	_heap const ax_list_node	*node;
	void 				*data;
} ax_list_iter_stack;
axres ax_list_iter(
	_in const ax_list 		*list,
	_in ax_iter_act			action,
	_in void 			*data,
	_out_opt const ax_list_node	**buf
);
axres ax_list_clear(
	_in ax_list 			*list
);
axres ax_list_delete(
	_in ax_list 			*list
);

#endif // !defined(AX_IO_STRUCTURES_LIST_INT)

#endif // !defined(AX_IO_STRUCTURES_INT)

