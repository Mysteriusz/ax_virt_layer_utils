#include "ax_parser.h"

const c16 *seq_spec_to_charset(
	_in const c16		*cpg // cpg FOR capture group
){
	// Follows <x-y>
	u32 rng = cpg[0]; // x
	u32 mode = cpg[1]; // -
	u32 rng_lim = cpg[2]; // y

	// Avaiable modes check
	if (rng == (u32)L'.' 
	&& mode != (u32)L'-'){
		return CHARSET_ANY;
	}

	c16 *set = axmalloc(((rng - rng_lim) + 1) * sizeof(c16));
	u32 i = 0;
	while(rng <= rng_lim){
		set[i] = (c16)rng;
		rng++;
		i++;
	}

	unref(mode);
	return set;
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

	const c16 *temp = nullptr;
	res = skip_until(fmt, FMT_GRP_SET, &temp);
	axcheck(res);

	u64 fmt_len = _c16len(fmt);
	u64 skip_len = 0;

	// Format specific characters
	const c16 *fmt_char = fmt;
	const c16 *spec_char = nullptr;

	// Sequence identifying characters
	c16 *rng = nullptr;
	u64 rng_len = 0;

/* 
	Iterate group 
	Example:
		Only the string between \\ will be read.
 		- L"\\[<.>]\\"
*/

	while(in_c16_s(fmt, fmt_char, fmt_len)
	&& *fmt_char != *FMT_GRP_SET){
		spec_char = fmt_char + 1;
		skip_len = 0;

		switch(*fmt_char){
		case L'<': // Capture group set
			res = skip_until(fmt_char, L">", &fmt_char);
			axcheck(res);

			const c16 *cap_set = seq_spec_to_charset(spec_char);
			grp->cap_sets->add(
				grp->cap_sets,
				(void*)cap_set,
				_c16len_b(cap_set) + sizeof(c16) // Include null-terminator
			); 

			skip_len = 1;
			break;
		default:
			// Add all non-needed as charset ranges

			read_until(fmt_char, L"<\\", &rng_len, rng);
			rng = axmalloc(rng_len * sizeof(c16)); 
			read_until(fmt_char, L"<\\", &rng_len, rng);

			grp->seq_list->add(
				grp->seq_list,
				rng,
				rng_len * sizeof(c16) // Include null-terminator
			);

			skip_len = rng_len - 1; 

			// Reset rng buffer after read
			rng = nullptr;
			rng_len = 0;
			break;
		}
		fmt_char += skip_len;
	}

	return AX_SUCC;
}

void* seq_split_fmt_iter(
	const ax_list list _prepass,
	const ax_list_node node _prepass
){
	fmt_group *grp = (fmt_group*)node->value;
	ax_list_delete(grp->cap_sets);
	ax_list_delete(grp->seq_list);

	return nullptr;	
}
axres seq_split_fmt(
	_in const c16 		*fmt,
	_out u32		*count,
	_out ax_list 		**grps
){
	if (fmt == nullptr){
		return AX_INV_FMT;
	}
	if (count == nullptr
	|| grps == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 fmt_len = _c16len(fmt);
	const c16 *fmt_char = fmt;
	const c16 *grp_char = nullptr;

	u32 grps_c = 0;

	while(in_c16_s(fmt, fmt_char, fmt_len)){
		res = skip_until(fmt_char, FMT_GRP_SET, &fmt_char);
		axcheck_b(res);

		grps_c++;
		fmt_char++;
	}
	grps_c--;

	ax_list *list = nullptr;
	res = ax_list_init(&list);
	axcheck(res);

	fmt_group *grp = axmalloc(sizeof(fmt_group));

	// Reset fmt_char
	fmt_char = fmt;
	for (u32 i = 0; i < grps_c; i++){
		fmt_char++;
		grp_char = fmt_char;

		// Find group ending
		res = skip_until(fmt_char, FMT_GRP_SET, &fmt_char);
		axcheck_b(res);

		// Initialize group lists
		res = ax_list_init(&grp->cap_sets);
		res = ax_list_init(&grp->seq_list);

		// Parse group
		res = seq_read_group(grp_char, dif_c16(grp_char, fmt_char), grp);
		axcheck_b(res);

		res = list->add(list, grp, sizeof(fmt_group));
		axcheck_b(res);
	}

	// Free the temp group buffer axfree(grp); Cleanup check
	axcheck(res,
		list->iter(list, (ax_structures_iter_act)seq_split_fmt_iter),
		list->delete(list)
	);

	*grps = list;

	return AX_SUCC;
}

axres seq_match(
	_in const c16		*text,
	_in const c16 		*seq_set,
	_in const c16 		*seq_end,
	_in const c16		*cap
){
	axres res = AX_SUCC;

	u64 v_set_s = 0;
	c16 *v_set = nullptr;

	u64 from = dif_c16(text, seq_set);
	u64 to = dif_c16(text, seq_end);

	res = read_range(text, from, to, &v_set_s, nullptr);
	axcheck(res);

	v_set = axmalloc(v_set_s * sizeof(c16));

	res = read_range(text, from, to, &v_set_s, v_set);
	axcheck(res, axfree(v_set));

	const c16 *v_set_loc = nullptr;
	res = skip_while(v_set, cap, &v_set_loc);
	axcheck(res, axfree(v_set));

	axfree(v_set);
	// Check if skipped distance was the correct size between start and end of the capture group sequence
	if (dif_c16(v_set, v_set_loc) != dif_c16(seq_set, seq_end)){
		return AX_INV_DATA;
	}

	return AX_SUCC;
}
axres seq_locate(
	_in const c16		*text,
	_in fmt_group 		*grp,
	_out const c16		**loc
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
	const c16 *set_char = text; 

	u32 seq_i = 0;
	u32 cap_i = 0;
	const c16 *seq_set = text_char;
	const c16 *seq_end = text_char;

	while(in_c16_s(text, text_char, text_len)
	&& seq_i < (grp->seq_list->count - 1)){
		// Find sequence start
		res = find_substr(
			text_char,
			i_as(grp->seq_list, seq_i, c16*),
			&seq_set,
			nullptr
		); 
		axcheck_b(res);

		// First check
		if (seq_i == 0){
			set_char = seq_set;
		}

		seq_set += _c16len(i_as(grp->seq_list, seq_i, c16*));
		seq_i++;
		
		// Find sequence end
		res = find_substr(
			seq_set,
			i_as(grp->seq_list, seq_i, c16*),
			&seq_end,
			nullptr
		); 
		axcheck_b(res);

		// Match with capture group
		res = seq_match(
			text_char,
			seq_set,
			seq_end, 
			i_as(grp->cap_sets, cap_i, c16*)
		);
		// Capture group failed
		// Reset search
		if(AX_ERR(res)){
			text_char = set_char + _c16len(i_as(grp->seq_list, 0, c16*));
			cap_i = 0;
			seq_i = 0;
			seq_set = text_char;
			seq_end = text_char;
			set_char = nullptr;
		}
		else{
			// Move to another sequence
			text_char = seq_end;
			cap_i++;
		}
	}

	if (set_char == nullptr){
		return AX_NOT_FND;
	}
	else axcheck(res);

	*loc = set_char;

	return AX_SUCC;
}

axres seq_find(
	_in const c16		*text,
	_in const c16 		*fmt,
	_out const c16 		**loc
){
	if (text == nullptr
	|| fmt == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	u32 grp_count = 0;
	ax_list *grp_list = nullptr;
	res = seq_split_fmt(fmt, &grp_count, &grp_list);
	axcheck(res);

	// Currently only one group
	const c16 *loc_buf = nullptr; 
	res = seq_locate(text, i_as(grp_list, 0, fmt_group*), &loc_buf);

	grp_list->iter(grp_list, (ax_structures_iter_act)seq_split_fmt_iter);
	axcheck(res);

	*loc = loc_buf;

	return AX_SUCC;
}

