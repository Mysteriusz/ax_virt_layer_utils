#include "ax_parser_seq.h"

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
	axfree((void*)cond->bef);
	axfree((void*)cond->aft);

	return ITER_NONE;
}
// fmt_var list cleanup iterator
iter_code fmt_var_iter(
	ax_list_iter_stack 	stack _prepass
){
	fmt_var *var = (fmt_var*)stack->node->value;
	axfree((void*)var->name);
	axfree((void*)var->value);

	return ITER_NONE;
}

_free c16 *_seq_read_range(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out u32		*skip
){
	if (fmt == nullptr
	|| fmt_char == nullptr
	|| skip == nullptr){
		return nullptr;
	}

	axres res = AX_SUCC;

	u32 fmt_len = _c16len(fmt);
	u32 skip_count = 0;

	c16 *prev_buf = nullptr;
	u32 prev_len = 0;

	c16 *rng_buf = nullptr;
	u32 rng_len = 0;

	// Check if next character is a functional character which is marked as escaped
	const c16 *beg_char = fmt_char;
	const c16 *spec_char = nullptr;
	while(in_c16_s(fmt, beg_char, fmt_len)){
		// Skip to first possible charset character occurence OR end of text
		res = skip_until(beg_char, CHARSET_SEQ UTF16_EOT_STR, &spec_char);
		axcheck_b(res);

		// No space in between last escape and next CHARSET_SEQ
		if(dif_c16(beg_char, spec_char) == 0){
			break;
		}

		// Get needed length for the new buffer and allocate
		rng_len = prev_len + dif_c16(beg_char, spec_char);
		rng_buf = axmalloc(rng_len * sizeof(c16));

		/*
		 	Copy memory from prev_buf and then from beg_char
			With range of size from beg_char to next CHARSET_SEQ occurence character
		*/
		if (prev_buf != nullptr){
			memcpy(rng_buf, prev_buf, prev_len * sizeof(c16));
			axfree(prev_buf);
		}
		memcpy(rng_buf + prev_len, beg_char, dif_c16(beg_char, spec_char) * sizeof(c16));

		// If next CHARSET_SEQ occurence is escaped then load it into the rng_buf as last and continue
		if (_is_esc(fmt, spec_char)){
			rng_buf[rng_len - 1] = *spec_char;
			skip_count++;
		}else{ // If not then break the loop and return output
			break;
		}

		prev_buf = rng_buf;
		prev_len = rng_len;

		beg_char = spec_char + 1;
	}

	*skip = skip_count + rng_len;

	return rng_buf;
}
axres seq_read_group(
	_in const c16		*fmt,
	_in_out fmt_group 	*grp
){
	if (fmt == nullptr){
		return AX_INV_ARG;
	}
	if (grp == nullptr
	|| grp->cond_list == nullptr
	|| grp->spec_list == nullptr
	|| grp->var_list == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u32 fmt_len = _c16len(fmt);
	const c16 *fmt_char = fmt;
	const c16 *fmt_beg = nullptr;

	// Sequence identifying characters
	c16 *rng = nullptr;
	u32 rng_len = 0;

	fmt_spec spec = {0};
	bool spec_add = false;

 	// Current processed count
	u32 spec_occ = 0;
	u32 var_occ = 0;
	u32 cond_occ = 0;
	unref(var_occ); unref(cond_occ);

/* 
	Iterate group 
	Example:
 		- u"(!:\".\")\\[^<{a-z}+{[-]}>$]"
*/
	while(in_c16_s(fmt, fmt_char, fmt_len)){
		fmt_beg = fmt_char;

		/*
			IMPORTANT!:
		 	Any case in this block will a stall if not handled and returned to the fmt_char.
		*/
		switch(*fmt_char){
		case u'^': // seq_loc start indicator
		case u'$': // seq_loc end indicator
			c16 *buf = axmalloc(sizeof(c16));
			*buf = *fmt_char;

			spec.value = buf;
			spec.type = (*fmt_char == u'^')
				? spec_control_beg
				: spec_control_end;

			fmt_char++;

			spec_add = true;
			break;
		case u'<': // Capture set
			res = seq_group_cap(fmt, fmt_char, &fmt_char, &spec);
			axcheck_b(res);
			spec_add = true;
			break;
		case u'[': // Variable
			res = seq_group_var(fmt, fmt_char, grp->var_list, spec_occ, &fmt_char);
			axcheck_b(res);
			var_occ++;
			break;
		case u'(': // Function
			res = seq_group_cond(fmt, fmt_char, grp->cond_list, &fmt_char);
			axcheck_b(res);
			cond_occ++;
			break;
		case u'?':
			fmt_char++;
			break;
		default:
			rng = _seq_read_range(fmt, fmt_char, &rng_len);
			axcheck_b((rng == nullptr));

			spec.value = rng;
			spec.type = spec_sequence;

			fmt_char += rng_len;

			spec_add = true;
			break;
		}

		// If AX_ERR(res) then spec->value is expected to not be allocated
		axcheck(res, io_i64(spec.type));

		// Set mode based on the first character of the sequnece (fmt_beg)
		spec.mode = (_is_opt(fmt, fmt_beg) == true)
			? spec_optional
			: spec_none;
		if (spec_add){
			if (spec.type == spec_sequence
			|| spec.type == spec_capture_set){
				spec_occ++;
			}

			// Add buffer specifier
			grp->spec_list->add(
				grp->spec_list,
				&spec,
				sizeof(fmt_spec)
			);
			spec_add = false;
		}
	}

	return AX_SUCC;
}

// Load variables from _fmt_group->var_list into dictionary
void grp_load_vars(
	_in fmt_group 		*grp,
	_in ax_dict 		*dict
){
	if(grp == nullptr
	|| dict == nullptr){
		return;
	}

	fmt_var *var = nullptr;
	for (u32 i = 0; i < grp->var_list->count; i++){
		var = index_as(grp->var_list, fmt_var*, i);
		asrt(var != nullptr);

		// Load variable name and value into the dictionary
		dict->add(
			dict,
			(void*)var->name,
			_c16len_b(var->name) + sizeof(c16),
			(void*)var->value,
			_c16len_b(var->value) + sizeof(c16)
		);
	}
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
	if (grp->var_list != nullptr){
		grp->var_list->iter(grp->var_list, (ax_iter_act)fmt_var_iter, nullptr, nullptr);
		grp->var_list->delete(grp->var_list);
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
	axcheck_g(res, error_jump);
	res = ax_list_init(&grp.cond_list);
	axcheck_g(res, error_jump);
	res = ax_list_init(&grp.var_list);
	axcheck_g(res, error_jump);

	// Parse group
	res = seq_read_group(fmt, &grp);
	axcheck_g(res, error_jump);

	*buf = grp;

	return AX_SUCC;

error_jump:
	grp_cleanup(&grp);
	return res;
}

axres seq_locate_nodet(
	_in const c16		*text,
	_in u32 		curr_i,
	_in ax_list		*spec_list,
	_out const c16		**loc,
	_out const c16		**beg_char, // Returned only in case of control_beg
	_out const c16		**end_char, // Returned only in case of control_end
	_out _free c16		**match_res,
	_out u32		*match_i
){
	if (text == nullptr
	|| spec_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr
	|| beg_char == nullptr
	|| end_char == nullptr
	|| match_res == nullptr
	|| match_i == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	const c16 *spec_beg = text;
	const c16 *spec_end = text;

	fmt_spec *curr = index_as(spec_list, fmt_spec*, curr_i);
	if (curr == nullptr){
		return AX_INV_MEM;
	}

	// Find next 'physical' sequence thats valid
	u64 next_i = curr_i + 1;
	fmt_spec *next = curr;
	while(curr->type == spec_capture_set
	&& next != nullptr){
		if (next->type == spec_sequence){
			break;
		}
		next = index_as(spec_list, fmt_spec*, next_i++);
	}

	/*
	 	Continous block skipping with respect for next->value
		
		Example:
			u"[[[secta]]]]" 
			WITH curr->value == u"[" 
			AND curr->type == spec_capture_set
			AND next == nullptr

			For block of u'[' it will skip all of its characters resulting in:
			u"secta]]]]"

			u"[[[secta]]]]" 
			WITH curr->value == u"[" 
			AND curr->type == spec_capture_set
			AND next != nullptr
			AND next->value == u"["

			The result would be:
			u"[secta]]]]"
			which considers next expected spec_sequence
	*/
	if (next != nullptr
	&& curr->type == spec_capture_set){
		// Skip until next->value
		while((contains(curr->value, *spec_end) == AX_SUCC)
		&& (starts_with(spec_end, next->value, nullptr) == AX_SUCC)){
			spec_end++;
		}

		// Check if next->value was only one character after
		if (spec_end != spec_beg){
			spec_end--;
		}else{
			// Find the ending position or move to next if optional
			res = find_substr(spec_beg, next->value, &spec_end, nullptr);
			axcheck_g(res, skip_jump);
		}
	}else if(curr->type == spec_capture_set){
		res = skip_while(spec_beg, curr->value, &spec_end);
		axcheck_g(res, skip_jump);
	}

	// Skip with respect to next
	switch(curr->type){
	case spec_sequence: // Match with spec_sequence
		res = starts_with(
			text,
			curr->value,
			nullptr
		);

		spec_end = text + _c16len(curr->value);
		break;
	case spec_capture_set: // Match with capture group
		// Edge case where there is no distance
		// Avoids AX_INV_IND
		if (spec_beg == spec_end){
			res = AX_NOT_FND;
			break;
		}
		res = seq_cap_match(
			text,
			curr->value,
			spec_beg,
			spec_end
		);
		break;
	case spec_control_beg:
		*beg_char = text;
		goto out_jump;
	case spec_control_end:
		*end_char = text;
		goto out_jump;
	default:
		break;
	}
	
skip_jump: // Skip to the function pre-write-back
	if (res == AX_NOT_FND 
	&& curr->mode == spec_optional){
		spec_end = spec_beg;
		goto out_jump; 
	}else{
		axcheck(res);
	}

	/*
	 	By this moment spec_beg and spec_end are valid and wont cause AX_INV_IND
	*/
	
	// Read the action processed range
	c16 *act_buf = nullptr;
	u32 act_len_n = 0;
	res = read_range(
		text,
		dif_c16(text, spec_beg),
		dif_c16(text, spec_end),
		&act_len_n,
		act_buf
	);
	axcheck(res);

	act_buf = axmalloc(act_len_n * sizeof(c16));

	res = read_range(
		text,
		dif_c16(text, spec_beg),
		dif_c16(text, spec_end),
		&act_len_n,
		act_buf
	);
	axcheck(res, axfree(act_buf));

	*match_res = act_buf;

out_jump: // Skip to the function write-back
	*loc = spec_end;

	// Update match index
	if (curr->type == spec_capture_set
	|| curr->type == spec_sequence){
		(*match_i)++;
	}

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

	u32 text_len = _c16len(text);
	const c16 *text_char = text;

	const c16 *root_char = text;
	const c16 *beg_char = text; // Indicated by ^ character
	const c16 *end_char = text; // Indicated by $ character

	u32 seq_i = 0;
	u32 match_i = 0;
	c16 *match_res = nullptr; // Buffer with result of seq_locate_action range

	while(in_c16_s(text, text_char, text_len)
	&& seq_i < grp->spec_list->count){
		// First check
		if (seq_i == 0){
			root_char = text_char;

			if (beg_char == nullptr){
				beg_char = text_char;
			}
		}

		// Match current text_char against conditions
		for (u32 i = 0; i < grp->cond_list->count; i++){
			res = seq_condition_match(
				text,
				text_char,
				index_as(grp->cond_list, fmt_cond*, i),
				&text_char
			);
			axcheck_g(res, skip_occ);
		}

redo_act:
		// Nondeterministic state branch
		res = seq_locate_nodet(
			text_char, 
			seq_i, 
			grp->spec_list, 
			&text_char,
			&beg_char,
			&end_char,
			&match_res,
			&match_i
		);
		axcheck_g(res, skip_occ);

		/*
		 	Everything in between redo_act AND skip_occ goto statements 
			is done after successfull state branch.
		*/

		// Process all active variables based on the match result range
		seq_var_process(
			grp->var_list,
			match_i - 1,
			match_res
		);

		// Match buffer cleanup
		axfree(match_res);
		match_res = nullptr;

		seq_i++;

		// Ensure all specifiers will be processed to end even when text_char reached null-terminator
		if (seq_i < grp->spec_list->count
		&& in_c16_s(text, text_char, text_len) == false){
			goto redo_act;
		}

		// Last iteration end_char write-back if not specified
		if (seq_i == grp->spec_list->count
		&& end_char == nullptr){
			end_char = text_char;
		}
		
skip_occ:
		// Reset search
		if(AX_ERR(res)){
			text_char = root_char + 1;
			seq_i = 0;
			match_i = 0;
			beg_char = nullptr;
			end_char = nullptr;
			root_char = nullptr;
		}
	}

	// Validate against seq_i and count of non-optional processed
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

	u32 text_len = _c16len(text); 
	const c16 *text_char = text; 
	seq_loc buf = {0};

	while(in_c16_s(text, text_char, text_len)){
		res = seq_locate(text_char, &grp, &buf);
		axcheck_b(res);

		// Add the spec_sequence occurrence to the list
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
axres seq_find_f(
	_in io_file		*file,
	_in const c16 		*fmt,
	_out seq_loc 		*loc
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (fmt == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	// fmap with file offset
	const c16 *fmap_off = file->map.root + file->offset;
	// uocation of substr in fmap
	seq_loc buf = {0};

	res = seq_find(fmap_off, fmt, &buf);
	axcheck(res);

	file->offset = (u64)buf.beg;
	//io_str((const c16*)file->offset);

	return AX_SUCC;
}
axres seq_find_all_f(
	_in io_file		*file,
	_in const c16 		*fmt,
	_in_out ax_list		*locs
){
	if (io_finv(file, UTF16)){
		return AX_INV_FILE;
	}
	if (fmt == nullptr){
		return AX_INV_ARG;
	}
	if (locs == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	// fmap with file offset
	const c16 *fmap_off = file->map.root;
	res = seq_find_all(fmap_off, fmt, locs);
	axcheck(res);

	return AX_SUCC;
}
