#include "ax_structures.h"

#if !defined(AX_STRUCTURES_DICT_INT)
#define AX_STRUCTURES_DICT_INT

typedef struct _ax_dict_node ax_dict_node;
typedef struct _ax_dict_node{
	void *key;
	u64 size;
} ax_dict_node;
typedef struct _ax_dict{
	enum ax_structure_type type;
	ax_dict_node *root;
	u32 count;
	u32 bucket_count; // Count of bucket entries from root
	struct { AX_STRUCTURE_CMD_ASC };
} ax_dict;

axres ax_dict_init(
	_in u32				exp_max, // Maximum expected count of values
	_out ax_dict			**buf
);
axres ax_dict_add(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size,
	_in void 			*value,
	_in u64 			value_size
);

#endif // !defined(AX_STRUCTURES_DICT_INT)

