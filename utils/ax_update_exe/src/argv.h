#include <ax_utility.h>

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
	struct argval *sub_args;
	u32 sub_args_c;
};

static struct argval ARG_MAP[] = {
	{  // Installation command
		u"-install",
		8,
		val_digit,
		(struct argval[]){
			(struct argval){u"-force", 0, val_switch, nullptr, 0},
		},
		1
	},
};
#define map_len (sizeof(ARG_MAP) / sizeof(struct argval))

i32 check_val(
	c16 		*value,
	struct argval	info
);
i32 check_arg(
	c16 		**argv,
	i32 		arg_i,
	i32 		val_i,
	i32 		argc,
	struct argval	map[],
	i32 		*skips
);

i32 check_argv(
	c16 		**argv,
	i32 		argc
);

