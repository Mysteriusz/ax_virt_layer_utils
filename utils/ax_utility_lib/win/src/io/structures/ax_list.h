#include "ax_structures.h"

#if !defined(AX_STRUCTURES_LIST_INT)
#define AX_STRUCTURES_LIST_INT

typedef struct _ax_list_node ax_list_node;
typedef struct _ax_list_node{
	void *value;
	u64 size;
	ax_list_node *next;
} ax_list_node;
typedef struct _ax_list{
	enum ax_structure_type type;
	u32 count;
	ax_list_node *root;
	struct { AX_STRUCTURE_CMD_SEQ };
} ax_list;

axres ax_list_init(
	_out ax_list			**buf
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
	_in ax_list 			*list,
	_in u32 			index,
	_out const ax_list_node 	**buf
);
const void *ax_list_query_at(
	_in ax_list			*structure,
	_in u32 			index
);

typedef struct _ax_list_iter_stack{
	const ax_list 			*list;
	const ax_list_node		*node;
	void 				*data;
} ax_list_iter_stack;
axres ax_list_iter(
	_in ax_list 			*list,
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

