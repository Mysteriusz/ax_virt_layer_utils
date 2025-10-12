#include "ax_parser.h"

enum set_mode{
	add = L'+', // Mathematical Union (U)
	collide = L'&', // Mathematical Intersect (∩)
	difference = L'-', // Mathematical Remove (\) 
};
c16 *_rng_to_set(
	const c16 a,
	const c16 b
){
	u32 c = (u32)a;
	u32 d = (u32)b;

	// Invert <z-a> -> <a-z>
	if (c > d){
		u32 temp = d;
		d = c;
		c = temp;
	}

	// Alloc space for set and write range
	c16 *set = axmalloc(((d - c) + 2) * sizeof(c16));
	u32 i = 0;
	while(c <= d){
		set[i] = (c16)c;
		c++;
		i++;
	}

	return set;
}

bool seq_cap_to_charset_inv(
	_in const c16		*cap // cap FOR capture group
){
	if (cap == nullptr){
		return true;
	}

	u64 cap_len = _c16len(cap);
	const c16 *cap_char = cap;

	if (cap[0] != L'{'){
		return true;
	}
	if (cap[cap_len - 1] != L'}'){
		return true;
	}

	const c16 *set_char = nullptr;
	const c16 *rng_char = nullptr;
	unref(rng_char);

	bool in_set = false;
	while(in_c16_s(cap, cap_char, cap_len)){
		switch(*cap_char){
		case L'{':
			if (in_set == false){
				set_char = cap_char;
			}
			in_set = true;
			break;
		case L'}':
			// Default set_char if exiting set
			if (in_set == true){
				set_char = nullptr;
			}
			in_set = false;
			break;
		case add:
		case difference:
		case collide:
			break;
		default:
			// Not in set and unknown character
			if (in_set == false){
				goto error_jump;
			}
			break;
		}

		// If cap_char in one of the sets
		if (in_set){
			set_char++;
			if (in_c16_s(cap, set_char, cap_len) == false){
				goto error_jump;
			}

			rng_char = set_char + 1;
			if (in_c16_s(cap, rng_char, cap_len) == false){
				goto error_jump;
			}

			switch(*rng_char){
			// Expect range: {a-z}
			case L'-':
				set_char = rng_char + 1; 
				if (in_c16_s(cap, set_char, cap_len) == false){
					goto error_jump;
				}

				break;
			// Expect one character: {a}
			case L'}':
				break;
			default:
				goto error_jump;
			}
			cap_char = set_char;

			if (*(cap_char + 1) != L'}'){
				goto error_jump;
			}
		}

		cap_char++;
	}
	
error_jump:
	// Not fully checked
	if (*cap_char != L'\0'){
		return true;
	}
	
	return false;
}

const c16 *seq_cap_to_charset(
	_in const c16		*cap // cap FOR capture group
){
	if (seq_cap_to_charset_inv(cap)){
		return nullptr;
	}

	u64 cap_len = _c16len(cap);
	const c16 *cap_char = cap;
	const c16 *not_char = nullptr;

	enum set_mode mode = 0;
	const c16 *charset = nullptr;
	// Set containing temp data to be merged with charset based on mode
	const c16 *wrkset = nullptr;

	// Capture set parse {a-z}-{c-m}+{l}
	while(in_c16_s(cap, cap_char, cap_len)){
		if (*cap_char != L'{'){
			cap_char++;
			continue;
		}
		if (charset != nullptr){
			mode = *(cap_char - 1);
		}

		not_char = cap_char;

		/* 
		 	Get the wrkset for operation.
			Index read is safe since the cap is validated by seq_cap_to_charset_inv.

			not_char[0] == L'{'
			not_char[1] == (from_range_char OR single_char)
			not_char[2] == (range_char OR L'}')
			not_char[3] == (to_range_char OR out_of_range)
			not_char[4] == (L'}' OR out_of_range)
		*/
		
		// Single char
		if (not_char[2] == L'}'){
			wrkset = _rng_to_set(not_char[1], not_char[1]);
			cap_char = &not_char[2];
		// Range
		}else{
			wrkset = _rng_to_set(not_char[1], not_char[3]);
			cap_char = &not_char[4];
		}

		/* 
		 	 Perform mode operation on the wrkset and charset.
		*/

		// Initialize charset on first iteration 
		if (charset == nullptr){
			charset = wrkset;
			continue;
		}

		c16 *temp_set = nullptr;
		u64 temp_size = 0;
		
		// Perform operation
		switch(mode){
		case add:
			c16_union(charset, wrkset, &temp_size, temp_set);
			temp_set = axmalloc(temp_size * sizeof(c16));
			c16_union(charset, wrkset, &temp_size, temp_set);
			break;
		case difference:
			c16_difference(charset, wrkset, &temp_size, temp_set);
			temp_set = axmalloc(temp_size * sizeof(c16));
			c16_difference(charset, wrkset, &temp_size, temp_set);
			break;
		default:
			break;
		}

		// Cleanup previous charset
		axfree((void*)charset);

		// Write to charset
		charset = temp_set;

		// Reset for next set
		mode = L'\0';
		axfree((void*)wrkset);
	}

	return charset;
}
axres seq_group_capture_set(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*spec_list,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr
	|| spec_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}
	if (*fmt_char != L'<'){
		return AX_INV_FMT;
	}

	axres res = AX_SUCC;

	fmt_char++;
	const c16 *spec_char = fmt_char;

	u64 spec_len = 0;
	c16 *spec_buf = nullptr;

	// Find ending of the capture group
	res = find_substr(fmt_char, L"}>", &spec_char, nullptr);
	axcheck(res);

	// Read inside of the capture group
	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char + 1),
		&spec_len,
		spec_buf
	);
	axcheck(res);
	
	spec_buf = axmalloc(spec_len * sizeof(c16));

	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char + 1),
		&spec_len,
		spec_buf
	);
	axcheck(res, axfree(spec_buf));

	const c16 *cap_set = seq_cap_to_charset(spec_buf);
	res = (cap_set != nullptr) 
		? AX_SUCC
		: AX_INV_FMT;
	axcheck(res, axfree(spec_buf));

	// Push to spec_list and offset fmt_char
	spec_list->add(
		spec_list,
		&(fmt_spec){
			.value = cap_set,
			.type = capture_set,
		},
		sizeof(fmt_spec)
	); 
	fmt_char += spec_len;

	// Cleanup
	axfree(spec_buf);

	// Write-back
	*loc = fmt_char;

	return AX_SUCC;
}

bool seq_func_to_cond_inv(
	_in const c16		*func // func for function
){
	if (func == nullptr){
		return true;
	}

	u64 func_len = _c16len(func);
	const c16 *func_char = func;

	// Check func bound characters
	if (*func_char != L'('){
		return true;
	}
	if (func_char[func_len - 1] != L')'){
		return true;
	}

	func_char++;
	if (in_c16_s(func, func_char, func_len) == false){
		return true;
	}

	// Get function return mode
	switch(*func_char){
	case L'!': // Not true statement
	case L'+': // True statement
		break;
	default: // Unknown statement value
		return true;
	}
	func_char++;

	if (in_c16_s(func, func_char, func_len) == false
	|| *func_char != L':'){
		return true;
	}
	func_char++;

	// Count special characters
	u64 c = 0;
	charset_count(func_char, SEQ_COND_CHARSET, &c);
	if (c == 0){
		return true;
	}

	return false;
}

axres seq_group_condition(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*spec_list,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr
	|| spec_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}
	if (*fmt_char != L'('){
		return AX_INV_FMT;
	}

	fmt_char++;

	*loc = fmt_char;

	return AX_SUCC;
}

