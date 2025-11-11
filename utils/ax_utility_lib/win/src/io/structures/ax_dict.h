#include "ax_structures.h"

#if !defined(AX_STRUCTURES_DICT_INT)
#define AX_STRUCTURES_DICT_INT

typedef struct _ax_dict_node ax_dict_node;
typedef struct _ax_dict_node{
	void *key;
	u64 key_size;
	void *value;
	u64 value_size;
	ax_dict_node *next; // Next node in node chain
} ax_dict_node;

typedef struct _ax_dict_chain{
	ax_dict_node *head;
} ax_dict_chain;

typedef struct _ax_dict{
	enum ax_structure_type type;
	ax_dict_chain *root;
	u32 count;
	u32 chain_count; // Count of chain entries from root
	struct { AX_STRUCTURE_CMD_ASC };
	struct { AX_STRUCTURE_OVR } overrides;
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
axres ax_dict_contains_key(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
);
axres ax_dict_remove(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
);
axres ax_dict_at(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size,
	_out const ax_dict_node		**buf	
);
const void *ax_dict_query_at(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
);
axres ax_dict_clear(
	_in ax_dict 			*dict
);
axres ax_dict_delete(
	_in ax_dict 			*dict
);

#endif // !defined(AX_STRUCTURES_DICT_INT)

