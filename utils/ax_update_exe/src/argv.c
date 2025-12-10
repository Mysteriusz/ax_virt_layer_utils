#include "argv.h"

i32 check_val(
	c16 		*value,
	struct argval	info
){
	if (value == nullptr){
		return -1;
	}

	// For bit_width arg is max 64bits (u64/i64)
	u16 req_width = info.bit_width;
	if (req_width == 0){
		return 0; // Early break (no need for width checks)
	}

	if (chkf(info.type, val_digit)){ // Validate type as a digit
		// Cast to i64
		c16 *e = nullptr;
		i64 val = _wcstoi64(value, &e, 0x10);
		if (req_width > 64 // Check agains long long bit width
		|| *e != u'\0'){ // Hasnt been fully parsed
			return -1;
		}

		// Adjust value for signed digits
		if (chkf(info.type, val_digit_sign)){
			req_width -= 1;
			if (val < 0){
				val = -val;
			}
		}

		// Check bit range of the value
		if (value_in_range(val, req_width, AX_UNK_ENDIAN) != 0){
			return -1;
		}	
	}else if (chkf(info.type, val_string)){ // Validate type as a string
		// For bit_width arg is any size
		if (wcslen(value) > (req_width * sizeof(c16))){
			return -1;
		}
	}

	return 0;
}

/*
 	Return values:
	1 -> arg is a marker (val ignored);
	2 -> arg and val are correct;
	-1 -> error/not found;
*/
i32 check_arg(
	c16 		**argv,
	i32 		arg_i,
	i32 		val_i,
	i32 		argc,
	struct argval	map[],
	i32 		*skips
){
	if (argv == nullptr
	|| arg_i >= argc
	|| skips == nullptr){
		return -1;
	}

	for (i32 i = 0; i < map_len; i++){
		if (compare(map[i].arg, argv[arg_i]) == 0){
			if (map[i].type == val_switch){
				// Return 1 if one checked
				*skips += 1;
			}else if (val_i >= argc){ // Impossible index of value and arg neeeds one
				return -1;
			}
			else{
				// Validate against val_type
				// Return 2 if both checked
				if (check_val(argv[val_i], map[i]) != 0){
					return -1;
				}
				*skips += 2;

			}

			// Sub-arg recursion
			if (map[i].sub_args_c > 0
			&& (arg_i + *skips < argc)){
				if (check_arg(
					argv,
					arg_i + *skips,
					val_i + *skips,
					argc,
					map[i].sub_args,
					skips) == -1){
					return -1;
				}
			}
			return 0;
		}
	}

	return -1;
}

i32 check_argv(
	c16		**argv,
	i32 		argc
){
	if (argv == nullptr){
		return -1;
	}

	i32 i = 1;
	i32 skips = 0;
	while (i < argc){
		if (check_arg(
			argv,
			i,
			i + 1,
			argc,
			ARG_MAP,
			&skips) == -1){
			return -1;	
		}

		i += skips;
	}

	return 0;
}


