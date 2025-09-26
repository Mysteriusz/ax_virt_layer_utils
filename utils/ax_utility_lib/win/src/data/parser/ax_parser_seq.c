#include "ax_parser.h"

enum set_mode{
	add = L'+', // Mathematical Union (U)
	collide = L'&', // Mathematical Intersect (∩)
	difference = L'-', // Mathematical Remove (\) 
};

c16 *_rng_to_set(
	const c16 *a,
	const c16 *b
){
	u32 c = (u32)*a;
	u32 d = (u32)*b;

	// Invert <c-d> -> <d-c>
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
const c16 *seq_spec_to_charset(
	_in const c16		*cap // cpg FOR capture group
){
	if (cap[0] != L'<'){
		return nullptr;
	}

	enum set_mode mode = 0;
	const c16 *charset = nullptr;
	// Set containing temp data to be merged with charset based on set_mode
	const c16 *wrkset = nullptr;
	unref(mode);
	unref(wrkset);

	const c16 *cap_end = cap;
	// Check and return nullptr if non-ending capture group
	axcheck_r(
		skip_until(cap, L">", &cap_end), nullptr
	);

	u64 cap_len = dif_c16(cap, cap_end) + 1;
	const c16 *cap_char = cap;
	const c16 *not_char = nullptr;
	const c16 *rng_char = nullptr;

	// Capture set parse <{a-z}-{c-m}+{l}>
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
		 	Get the wrkset for operation 
		*/

		// Check if range in current notation
		skip_until(not_char, L"}-", &rng_char);

		// Range check
		if (*rng_char == L'-'){ 
			wrkset = _rng_to_set(rng_char - 1, rng_char + 1);
		// Sentinel char (CHARSET_ANY; no range expected)
		}else if (*rng_char == L'}' 
		&& *(rng_char + 1) == L'.'
		&& *(rng_char - 1) == L'.'){ 
			wrkset = CHARSET_ANY;
		// Means its a single char
		}else if (*rng_char == L'}'){ 	
			wrkset = _rng_to_set(rng_char - 1, rng_char - 1);
		// Empty set
		}else{ 
			wrkset = nullptr;
		}

		/* 
		 	 Perform mode operation on the wrkset and charset
		*/

		axcheck_r(
			skip_until(cap_char, L"}", &cap_char), nullptr
		);

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

			axfree((void*)charset);
			break;
		default:
			break;
		}

		// Write to charset
		charset = temp_set;

		// Reset for next set
		mode = L'\0';
		axfree((void*)wrkset);
	}

	return charset;
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

	u64 fmt_len = _c16len(fmt);
	const c16 *fmt_char = fmt;

	// Sequence identifying characters
	c16 *rng = nullptr;
	u64 rng_len = 0;

/* 
	Iterate group 
	Example:
		Only the string between \\ will be read.
 		- L"\\[<.>]\\"
*/

	while(in_c16_s(fmt, fmt_char, fmt_len)){
		if (*fmt_char == L'\\'){
			fmt_char++;
			continue;
		}

		switch(*fmt_char){
		case L'<': // Capture group set
			const c16 *cap_set = seq_spec_to_charset(fmt_char);
			grp->cap_sets->add(
				grp->cap_sets,
				(void*)cap_set,
				_c16len_b(cap_set) + sizeof(c16) // Include null-terminator
			); 

			skip_until(fmt_char, L">", &fmt_char);
			
			fmt_char += 1;
			break;
		default:
			// Reset
			rng = nullptr;
			rng_len = 0;

			// Add all non-needed as charset ranges
			read_until(fmt_char, L"<\\", &rng_len, rng);
			rng = axmalloc(rng_len * sizeof(c16)); 
			read_until(fmt_char, L"<\\", &rng_len, rng);

			grp->seq_list->add(
				grp->seq_list,
				rng,
				rng_len * sizeof(c16) // Include null-terminator
			);

			// Without null-terminator
			fmt_char += (rng_len - 1);
			break;
		}
	}

	return AX_SUCC;
}

void seq_split_fmt_iter(
	const ax_list list _prepass,
	const ax_list_node node _prepass
){
	fmt_group *grp = (fmt_group*)node->value;
	ax_list_delete(grp->cap_sets);
	ax_list_delete(grp->seq_list);

	return;	
}
axres seq_split_fmt(
	_in const c16 		*fmt,
	_out ax_list 		**grps
){
	if (fmt == nullptr){
		return AX_INV_FMT;
	}
	if (grps == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	const c16 *fmt_char = fmt;
	const c16 *grp_char = nullptr;

	// Count group chars
	u32 grps_c = 0;
	res = charset_count(fmt, FMT_GRP, &grps_c);
	if (grps_c < 2){
		res = AX_INV_FMT;
	}

	axcheck(res);
	grps_c /= 1.5f;

	ax_list *list = nullptr;
	res = ax_list_init(&list);
	axcheck(res);

	fmt_group *grp = axmalloc(sizeof(fmt_group));
	for (u32 i = 0; i < grps_c; i++){
		grp_char = fmt_char;

		// Find group ending
		res = skip_until(fmt_char, FMT_GRP, &fmt_char);
		axcheck_b(res);

		// Initialize group lists
		res = ax_list_init(&grp->cap_sets);
		res = ax_list_init(&grp->seq_list);

		// Parse group
		res = seq_read_group(grp_char, dif_c16(grp_char, fmt_char), grp);
		axcheck_b(res);

		res = list->add(list, grp, sizeof(fmt_group));
		axcheck_b(res);

		fmt_char++;
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
	_in const c16 		*seq_beg,
	_in const c16 		*seq_end,
	_in const c16		*cap
){
	axres res = AX_SUCC;

	u64 inv_set_s = 0;
	c16 *inv_beg = nullptr;

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
	const c16 *seq_beg = text_char;
	const c16 *seq_end = text_char;

	while(in_c16_s(text, text_char, text_len)
	&& seq_i < (grp->seq_list->count - 1)){
		// Find sequence start
		res = find_substr(
			text_char,
			index_as(grp->seq_list, seq_i, c16*),
			&seq_beg,
			nullptr
		); 
		axcheck_b(res);

		// First check
		if (seq_i == 0){
			set_char = seq_beg;
		}

		seq_beg+= _c16len(index_as(grp->seq_list, seq_i, c16*));
		seq_i++;
		
		// Find sequence end
		res = find_substr(
			seq_beg,
			index_as(grp->seq_list, seq_i, c16*),
			&seq_end,
			nullptr
		); 
		axcheck_b(res);

		// Match with capture group
		res = seq_match(
			text_char,
			seq_beg,
			seq_end, 
			index_as(grp->cap_sets, cap_i, c16*)
		);
		// Capture group failed
		// Reset search
		if(AX_ERR(res)){
			text_char = set_char + _c16len(index_as(grp->seq_list, 0, c16*));
			seq_beg = text_char;
			seq_end = text_char;
			cap_i = 0;
			seq_i = 0;
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
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	ax_list *grp_list = nullptr;
	res = seq_split_fmt(fmt, &grp_list);
	axcheck(res);

	// Currently only one group
	const c16 *text_loc = nullptr; 
	res = seq_locate(text, index_as(grp_list, 0, fmt_group*), &text_loc);

	// Iterate grp_list cleanup function
	grp_list->iter(grp_list, (ax_structures_iter_act)seq_split_fmt_iter);
	axcheck(res);

	*loc = text_loc;

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

	ax_list *grp_list = nullptr;
	res = seq_split_fmt(fmt, &grp_list);
	axcheck(res);

	u64 text_len = _c16len(text); 
	const c16 *text_char = text; 
	const c16 *text_loc = text; 

	// Currently only first group
	fmt_group* grp = index_as(grp_list, 0, fmt_group*);
	while(in_c16_s(text, text_char, text_len)){
		res = seq_locate(text_char, grp, &text_loc);
		axcheck_b(res);

		// Add the sequence occurrence to the list
		locs->add(locs, &text_loc, sizeof(const c16*));
		text_char = text_loc + _c16len(
			index_as(
				grp->seq_list,
				0,
				c16*
			)
		);
		io_i64(_c16len(index_as(grp->seq_list, 0, c16*)));
	}

	// No occurrences added and res is not internal err
	if (locs->count != 0
	&& res == AX_NOT_FND){
		res = AX_SUCC;
	}

	// Iterate grp_list cleanup function
	grp_list->iter(grp_list, (ax_structures_iter_act)seq_split_fmt_iter);
	axcheck(res, locs->clear(locs));

	return AX_SUCC;
}

