#if !defined(AX_IO_STRUCTURES_INT)
#define AX_IO_STRUCTURES_INT

#include "ax_type.h"
#include "ax_error_code.h"
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
typedef axres (*ax_structures_at)(
	_in const void 			*structure,
	_in u32 			index,
	_out const void 		**structure_desc
);
typedef axres (*ax_structures_delete)(
	_in void 			*structure
);

#if !defined(AX_IO_STRUCTURES_LIST_INT)
#define AX_IO_STRUCTURES_LIST_INT

typedef struct _ax_list_node ax_list_node;
typedef struct _ax_list_node{
	void *value;
	u64 size;
	ax_list_node *next;
} ax_list_node;
typedef struct _ax_list{
	ax_list_node *root;
	u32 count;
	ax_structures_add add;
	ax_structures_remove remove;
	ax_structures_at at;
	ax_structures_delete delete;
} ax_list;

axres ax_list_init(
	_out ax_list			**buf
);
axres ax_list_add(
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
axres ax_list_delete(
	_in ax_list 			*list
);

#endif // !defined(AX_IO_STRUCTURES_LIST_INT)

#endif // !defined(AX_IO_STRUCTURES_INT)

