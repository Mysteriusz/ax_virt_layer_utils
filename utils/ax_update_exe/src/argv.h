#include "ax_utility.h"
#include "resolver.h"

enum val_type : u32{
	val_switch = 0, // bool (no value just the argument)
	val_string = 1, // text
	val_digit = 2, // unsigned (+) digit
	val_digit_sign = val_digit | 4, // signed (+/-) digit
};

struct argval{
	c16 *arg;
	u16 bit_width;
	enum val_type type;
	u32 sub_args_c;
	struct argval *sub_args;
	struct op_desc op_desc;
};

static struct argval ARG_MAP[] = {
	{  // Installation command
		u"--install",
		0,
		val_switch,
		1,
		(struct argval[]){
			// Installation configuration file path
			(struct argval){u"-config", 0, val_string, 0, nullptr, {0}},
		},
		(struct op_desc){
			.type = OP_INSTALL,
			.call = op_install_call,
			.meta = (struct op_meta){0},
		}
	},
};
#define map_len (sizeof(ARG_MAP) / sizeof(struct argval))

i32 check_val(
	c16 		*value,
	struct argval	info
);
i32 check_arg(
	_in c16 		**argv,
	_in i32 		arg_i,
	_in i32 		val_i,
	_in i32 		argc,
	_in struct argval	map[],
	_out i32 		*skips,
	_out struct op_desc 	*info
);

i32 check_argv(
	_in c16			**argv,
	_in i32 		argc,
	_in ax_list 		*op_list // Access by index_as(ops, op_desc, index);
);

