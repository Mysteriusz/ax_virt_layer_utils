#include "ax_parser.h"

// fmt_spec list cleanup iterator
iter_code fmt_spec_iter(
	ax_list_iter_stack 	stack _prepass
){
	fmt_spec *spec = (fmt_spec*)stack->node->value;
	axfree((void*)spec->value);

	return ITER_NONE;
}
// fmt_cond list cleanup iterator
iter_code fmt_cond_iter(
	ax_list_iter_stack 	stack _prepass
){
	fmt_cond *cond = (fmt_cond*)stack->node->value;
	if (chkf(cond->mode, condition_bef)) {
		axfree((void*)cond->bef);
	}
	if (chkf(cond->mode, condition_aft)) {
		axfree((void*)cond->aft);
	}

	return ITER_NONE;
}

axres seq_read_group(
	_in const c16		*fmt,
	_in u64 		grp_len,
	_in_out fmt_group 	*grp
){
	if (fmt == nullptr){
		return AX_INV_ARG;
	}
	if (grp == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 fmt_len = _c16len(fmt);
	const c16 *fmt_char = fmt;

	// Sequence identifying characters
	c16 *rng = nullptr;
	u64 rng_len = 0;

/* 
	Iterate group 
	Example:
		Only the string between \\ will be read.
 		- L"\\(!:\"$\")[\x2<{a-z}+{[-]}>\x3]\\"
*/
	while(in_c16_s(fmt, fmt_char, fmt_len)){
		switch(*fmt_char){
		case L'<': // Capture set
			res = seq_group_capture_set(fmt, fmt_char, grp->spec_list, &fmt_char);
			if (AX_ERR(res)) { goto error_jump; }
			break;
		case L'\x2': // seq_loc start indicator
		case L'\x3': // seq_loc end indicator
			// Load character to fmt_spec as control
			c16 *buf = axmalloc(sizeof(c16));
			*buf = *fmt_char;
			grp->spec_list->add(
				grp->spec_list,
				&(fmt_spec){
					.value = buf,
					.type = (*fmt_char == L'\x2' ? control_beg : control_end),
				},
				sizeof(fmt_spec)
			);
			fmt_char++;
			break;
		case L'(': // Condition start
			res = seq_group_condition(fmt, fmt_char, grp->cond_list, &fmt_char);
			if (AX_ERR(res)) { goto error_jump; }
			break;
		default:
			// Reset
			rng = nullptr;
			rng_len = 0;

			// Count for next sequences and if there arent any read to end
			u64 rng_c = 0;
			charset_count(fmt_char, CHARSET_SEQ, &rng_c);
			const c16 *rng_set = (rng_c == 0)
				? L""
				: CHARSET_SEQ; 

			// Add all non-needed as charset ranges
			res = read_until(fmt_char, rng_set, &rng_len, rng);
			if (AX_ERR(res)){ goto error_jump; }

			rng = axmalloc(rng_len * sizeof(c16)); 

			res = read_until(fmt_char, rng_set, &rng_len, rng);
			if (AX_ERR(res)){ goto error_jump; }

			grp->spec_list->add(
				grp->spec_list,
				&(fmt_spec){
					.value = rng,
					.type = sequence,
				},
				sizeof(fmt_spec)
			);

			// Without null-terminator
			fmt_char += (rng_len - 1);
			break;
		}
	}

error_jump:
	axcheck(res);

	return AX_SUCC;
}

// Cleanup _fmt_group
void grp_cleanup(
	_in fmt_group 		*grp
){
	if (grp == nullptr){
		return;
	}

	if (grp->spec_list != nullptr){
		grp->spec_list->iter(grp->spec_list, (ax_iter_act)fmt_spec_iter, nullptr, nullptr);
		grp->spec_list->delete(grp->spec_list);
	}
	if (grp->cond_list != nullptr){
		grp->cond_list->iter(grp->cond_list, (ax_iter_act)fmt_cond_iter, nullptr, nullptr);
		grp->cond_list->delete(grp->cond_list);
	}
}
axres seq_split_fmt(
	_in const c16 		*fmt,
	_in_out fmt_group 	*buf
){
	if (fmt == nullptr){
		return AX_INV_FMT;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	fmt_group grp = {0};

	// Initialize group lists
	res = ax_list_init(&grp.spec_list);
	axcheck(res);
	res = ax_list_init(&grp.cond_list);
	axcheck(res, grp_cleanup(&grp));

	// Parse group
	res = seq_read_group(fmt, _c16len(fmt), &grp);
	axcheck(res, grp_cleanup(&grp));

	*buf = grp;

	return AX_SUCC;
}

axres seq_match(
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

	u64 inv_set_s = 0;
	c16 *inv_beg = nullptr;

	if (seq_end == nullptr){
		res = skip_until(text, cap, &seq_end);
		axcheck(res);
	}

	u64 from = dif_c16(text, seq_beg);
	u64 to = dif_c16(text, seq_end);

	// Read range
	res = read_range(text, from, to, &inv_set_s, nullptr);
	axcheck(res);

	inv_beg = axmalloc(inv_set_s * sizeof(c16));

	res = read_range(text, from, to, &inv_set_s, inv_beg);
	axcheck(res, axfree(inv_beg));

	// Try to skip entire inv_begwith capture group
	const c16 *inv_set_loc = nullptr;
	res = skip_while(inv_beg, cap, &inv_set_loc);
	axcheck(res, axfree(inv_beg));

	axfree(inv_beg);
	// Check if skip count was as seq_end expects
	if (dif_c16(inv_beg, inv_set_loc) != dif_c16(seq_beg, seq_end)){
		return AX_INV_DATA;
	}

	return AX_SUCC;
}
axres seq_match_conditions(
	_in const c16		*text,
	_in ax_list		*cond_list,
	_out const c16		**loc
){
	if (text == nullptr
	|| cond_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;

	fmt_cond *curr = nullptr;
	for (u64 i = 0; i < cond_list->count; i++){
		curr = index_as(cond_list, i, fmt_cond*);

		if (chkf(curr->mode, condition_bef)){
			find_substr(text_char, curr->bef, &text_char, nullptr);
			text_char += _c16len(curr->bef);
		}
		if (chkf(curr->mode, condition_aft)){
			find_substr(text_char, curr->aft, &text_char, nullptr);
			text_char += _c16len(curr->aft);
		}
	}

	*loc = text_char;

	return AX_SUCC;
}
axres seq_locate_action(
	_in const c16		*text,
	_in u64 		curr_i,
	_in ax_list		*spec_list,
	_out const c16		**loc,
	_out const c16		**beg_char, // Returned only in case of control_beg
	_out const c16		**end_char // Returned only in case of control_end
){
	if (text == nullptr
	|| spec_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr
	|| beg_char == nullptr
	|| end_char == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	const c16 *spec_beg = text;
	const c16 *spec_end = text;

	fmt_spec *curr = index_as(spec_list, curr_i, fmt_spec*);

	// Find next 'physical' type sequence to validate with capture_set
	u64 next_i = curr_i + 1;
	fmt_spec *next = curr;
	while(curr->type == capture_set
	&& next != nullptr){
		if (next->type == sequence){
			break;
		}
		next = index_as(spec_list, next_i++, fmt_spec*);
	}

	/*
	 	Continous block skipping with respect for next->value
		
		Example:
			L"[[[secta]]]]" 
			WITH curr->value == L"[" 
			AND curr->type == capture_set
			AND next == nullptr

			For block of L'[' it will skip all of its characters resulting in:
			L"secta]]]]"

			L"[[[secta]]]]" 
			WITH curr->value == L"[" 
			AND curr->type == capture_set
			AND next != nullptr
			AND next->value == L"["

			The result would be:
			L"[secta]]]]"
			which considers next expected sequence
	*/
	if (next != nullptr
	&& curr->type == capture_set){
		while((contains(curr->value, *spec_end) == AX_SUCC)
		&& (starts_with(spec_end, next->value, nullptr) == AX_SUCC)){
			spec_end++;
		}

		if (spec_end != spec_beg){
			spec_end--;
		}else{
			res = find_substr(spec_beg, next->value, &spec_end, nullptr);
			axcheck(res);
		}
	}else if(curr->type == capture_set){
		res = skip_while(spec_beg, curr->value, &spec_end);
		axcheck(res);
	}

	// Skip with respect to next
	switch(curr->type){
	case sequence: // Match with sequence
		res = starts_with(
			text,
			curr->value,
			nullptr
		);

		spec_end = text + _c16len(curr->value);
		break;
	case capture_set: // Match with capture group
		res = seq_match(
			text,
			curr->value,
			spec_beg,
			spec_end
		);
		break;
	case control_beg:
		*beg_char = text;
		break;
	case control_end:
		*end_char = text;
		break;
	default:
		break;
	}
	
	*loc = spec_end;
	axcheck(res);

	return AX_SUCC;
}
axres seq_locate(
	_in const c16		*text,
	_in fmt_group 		*grp,
	_out seq_loc		*loc
){
	if (text == nullptr
	|| grp == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 text_len = _c16len(text);
	const c16 *text_char = text;

	const c16 *root_char = text;
	const c16 *beg_char = text; // Indicated by \0x2 character or seq beg
	const c16 *end_char = text; // Indicated by \0x3 character or seq end

	u32 seq_i = 0;

	while(in_c16_s(text, text_char, text_len)
	&& seq_i < grp->spec_list->count){
		// First check
		if (seq_i == 0){
			root_char = text_char;
			if (beg_char == nullptr){
				beg_char = text_char;
			}

			// Match current text_char against conditions
			res = seq_match_conditions(
				text_char,
				grp->cond_list,
				&text_char
			);
			axcheck(res);
		}

		// Move text_char based on the curr and next
		res = seq_locate_action(
			text_char, 
			seq_i, 
			grp->spec_list, 
			&text_char,
			&beg_char,
			&end_char
		);
		axcheck_g(res, skip_occ);
		
		seq_i++;
		if (seq_i == grp->spec_list->count
		&& end_char == nullptr){
			end_char = text_char;
		}

skip_occ:
		// Reset search
		if(AX_ERR(res)){
			text_char = root_char + 1;
			seq_i = 0;
			beg_char = nullptr;
			end_char = nullptr;
			root_char = nullptr;
		}
	}

	if (seq_i < grp->spec_list->count){
		return AX_NOT_FND;
	}
	else axcheck(res);

	loc->beg = beg_char;
	loc->end = end_char;

	return AX_SUCC;
}

axres seq_find(
	_in const c16		*text,
	_in const c16 		*fmt,
	_out seq_loc 		*loc
){
	if (text == nullptr
	|| fmt == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	fmt_group grp = {0};
	res = seq_split_fmt(fmt, &grp);
	axcheck(res);

	seq_loc buf = {0};
	res = seq_locate(text, &grp, &buf);
	grp_cleanup(&grp);

	axcheck(res);

	*loc = buf;

	return AX_SUCC;
}
axres seq_find_all(
	_in const c16		*text,
	_in const c16 		*fmt,
	_in_out ax_list		*locs
){
	if (text == nullptr
	|| fmt == nullptr){
		return AX_INV_ARG;
	}
	if (locs == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	fmt_group grp = {0};
	res = seq_split_fmt(fmt, &grp);
	axcheck(res);

	u64 text_len = _c16len(text); 
	const c16 *text_char = text; 
	seq_loc buf = {0};

	while(in_c16_s(text, text_char, text_len)){
		res = seq_locate(text_char, &grp, &buf);
		axcheck_b(res);

		// Add the sequence occurrence to the list
		locs->add(locs, &buf, sizeof(seq_loc));
		text_char = buf.beg + 1;
	}

	// No occurrences added and res is not internal error
	if (locs->count != 0
	&& res == AX_NOT_FND){
		res = AX_SUCC;
	}

	grp_cleanup(&grp);
	axcheck(res, locs->clear(locs));

	return AX_SUCC;
}

