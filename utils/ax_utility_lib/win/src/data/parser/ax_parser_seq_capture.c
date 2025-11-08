/*
 	Sequence finder capture set interface
*/

#include "ax_parser_seq.h"

enum set_mode{
	add = u'+', // Mathematical Union (U)
	collide = u'&', // Mathematical Intersect (∩)
	difference = u'-', // Mathematical Remove (\) 
};
_free c16 *_rng_to_set(
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

	u32 cap_len = _c16len(cap);
	const c16 *cap_char = cap;

	if (cap[0] != u'{'){
		return true;
	}
	if (cap[cap_len - 1] != u'}'){
		return true;
	}

	const c16 *set_char = nullptr;
	const c16 *rng_char = nullptr;

	bool in_set = false;
	while(in_c16_s(cap, cap_char, cap_len)){
		switch(*cap_char){
		case u'{':
			if (in_set == false){
				set_char = cap_char;
			}
			in_set = true;
			break;
		case u'}':
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
			case u'-':
				set_char = rng_char + 1; 
				if (in_c16_s(cap, set_char, cap_len) == false){
					goto error_jump;
				}

				break;
			// Expect one character: {a}
			case u'}':
				break;
			default:
				goto error_jump;
			}
			cap_char = set_char;

			if (*(cap_char + 1) != u'}'){
				goto error_jump;
			}
		}

		cap_char++;
	}
	
error_jump:
	// Not fully checked
	if (*cap_char != u'\0'){
		return true;
	}
	
	return false;
}

_free c16 *_seq_cap_to_charset(
	_in const c16		*cap // cap FOR capture group
){
	if (seq_cap_to_charset_inv(cap)){
		return nullptr;
	}

	u32 cap_len = _c16len(cap);
	const c16 *cap_char = cap;
	const c16 *not_char = nullptr;

	enum set_mode mode = 0;
	c16 *charset = nullptr;
	// Set containing temp data to be merged with charset based on mode
	c16 *wrkset = nullptr;

	// Capture set parse {a-z}-{c-m}+{l}
	while(in_c16_s(cap, cap_char, cap_len)){
		if (charset != nullptr){
			mode = *(cap_char - 1);
		}

		not_char = cap_char;

		/* 
		 	Get the wrkset for operation.
			Index read is safe since the cap is validated by seq_cap_to_charset_inv.

			not_char[0] == u'{'
			not_char[1] == (from_range_char OR single_char)
			not_char[2] == (range_char OR u'}')
			not_char[3] == (to_range_char OR out_of_range)
			not_char[4] == (u'}' OR out_of_range)
		*/
		
		// Single char
		if (not_char[2] == u'}'){
			c16 set_val = not_char[1];
			if (set_val == u'.'){
				set_val = UTF16_ANY;
			}

			wrkset = _rng_to_set(set_val, set_val);
			cap_char = &not_char[2];
		// Range
		}else{
			wrkset = _rng_to_set(not_char[1], not_char[3]);
			cap_char = &not_char[4];
		}
		// Skip '}' and next mode character
		cap_char += 2;

		/* 
		 	 Perform mode operation on the wrkset and charset.
		*/

		// Initialize charset on first iteration 
		if (charset == nullptr){
			charset = wrkset;
			continue;
		}

		c16 *temp_set = nullptr;
		u32 temp_len_n = 0;
		
		/*
		 	Perform operation
		*/
		switch(mode){
		case add:
			c16_union(charset, wrkset, &temp_len_n, temp_set);
			temp_set = axmalloc(temp_len_n * sizeof(c16));
			c16_union(charset, wrkset, &temp_len_n, temp_set);
			break;
		case difference:
			c16_difference(charset, wrkset, &temp_len_n, temp_set);
			temp_set = axmalloc(temp_len_n * sizeof(c16));
			c16_difference(charset, wrkset, &temp_len_n, temp_set);
			break;
		default:
			break;
		}

		// Cleanup previous charset
		axfree((void*)charset);

		// Write to charset
		charset = temp_set;

		// Reset for next set
		mode = u'\0';
		axfree((void*)wrkset);
	}

	return charset;
}
axres seq_group_cap_end(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	u32 fmt_len = _c16len(fmt);
	const c16 *spec_char = fmt_char;

	bool in_set = false;

	while(in_c16_s(fmt, spec_char, fmt_len)){
		switch(*spec_char){
		case u'{':
			if (in_set == false
			&& !_is_esc(fmt, spec_char)){
				in_set = true;
			}
			break;
		case u'}':
			if (in_set == true
			&& !_is_esc(fmt, spec_char)){
				in_set = false;
			}
			break;
		case u'>':
			if (in_set == false
			&& !_is_esc(fmt, spec_char)){
				goto exit_jump;
			}
			break;
		}
		spec_char++;
	}
exit_jump:

	if (*spec_char != u'>'){
		return AX_NOT_FND;
	}

	*loc = spec_char;
	return AX_SUCC;
}
axres seq_group_cap(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_out const c16		**loc,
	_out fmt_spec		*buf
){
	if (fmt == nullptr
	|| fmt_char == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr
	|| buf == nullptr){
		return AX_INV_BUF;
	}
	if (*fmt_char != u'<'){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	const c16 *spec_char = nullptr;

	u32 spec_len = 0;
	c16 *spec_buf = nullptr;

	// Skip initial u'<'
	fmt_char++;

	// Find ending of the capture set
	res = seq_group_cap_end(fmt, fmt_char, &spec_char);
	axcheck(res);

	/* 
		Read range for the capture set
	*/
	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len,
		spec_buf
	);
	axcheck(res);
	
	spec_buf = axmalloc(spec_len * sizeof(c16));

	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len,
		spec_buf
	);
	axcheck(res, axfree(spec_buf));

	c16 *cap_set = _seq_cap_to_charset(spec_buf);
	axfree(spec_buf);

	axcheck_r((cap_set == nullptr), AX_INV_FMT);

	// Write-back
	buf->value = cap_set;
	buf->type = spec_capture_set;

	// Move to end + 1 since we want to skip last u')' character too
	fmt_char = spec_char + 1;

	// Write-back
	*loc = fmt_char;

	return AX_SUCC;
}

axres seq_cap_match(
	_in const c16		*text,
	_in const c16		*cap,
	_in const c16 		*seq_beg,
	_in_opt const c16 	*seq_end
){
	if(text == nullptr
	|| cap == nullptr 
	|| seq_beg == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	u32 set_len_n = 0;
	c16 *set_buf = nullptr;

	if (seq_end == nullptr){
		res = skip_until(text, cap, &seq_end);
		axcheck(res);
	}

	u32 from = dif_c16(text, seq_beg);
	u32 to = dif_c16(text, seq_end);

	// Read range
	res = read_range(text, from, to, &set_len_n, set_buf);
	axcheck(res);

	set_buf = axmalloc(set_len_n * sizeof(c16));

	res = read_range(text, from, to, &set_len_n, set_buf);
	axcheck(res, axfree(set_buf));

	// Try to skip entire inv_begwith capture group
	const c16 *set_char = nullptr;
	res = skip_while(set_buf, cap, &set_char);
	axcheck(res, axfree(set_buf));

	axfree(set_buf);
	// Check if skip count was as seq_end expects
	if (dif_c16(set_buf, set_char) != dif_c16(seq_beg, seq_end)){
		return AX_NOT_FND;
	}

	return AX_SUCC;
}

