#include "ax_utility.h"

enum op_type : u16{
	OP_INSTALL = 1
};
struct op_meta{
	bool force;
	c16 *cfg_uri;
};
struct op_err{
	axres 		op_code;
	const c16 	*op_name; // Name of the operation that threw an error
};
struct op_desc{
	enum op_type type;
	struct op_meta meta;
	struct op_err (*call)(struct op_meta*);
};

struct op_err op_install_call(
	struct op_meta 		*meta
);

