#include "ax_parser.h"

const c16 *seq_group_to_charset(
	_in const c16		*cpg
){
	if (*cpg == L'.'){
		return CHARSET_ANY;
	}

	return nullptr;
}
#define CHARSET_SEQ 			L"<\\"
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

	while(in_c16_s(fmt, fmt_char, fmt_len)
	&& *fmt_char != *FMT_GRP_SET){
		spec_char = fmt_char + 1;
		skip_len = 0;

		switch(*fmt_char){
		case L'<': // Capture group set
			res = skip_until(fmt_char, L">", &fmt_char);
			axcheck(res);

			const c16 *cap_set = seq_group_to_charset(spec_char);
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
axres seq_split_fmt(
	_in const c16 		*fmt,
	_out u32		*count,
	_out fmt_group 		**grps
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

	fmt_group *grps_arr = axmalloc(sizeof(fmt_group) * grps_c);
	u32 grps_i = 0;

	// Reset fmt_char
	fmt_char = fmt;
	for (u32 i = 0; i < grps_c; i++){
		fmt_char++;
		grp_char = fmt_char;

		// Find group ending
		res = skip_until(fmt_char, FMT_GRP_SET, &fmt_char);
		axcheck_b(res, axfree(grps_arr));

		// Initialize group lists
		ax_list_init(&grps_arr[grps_i].cap_sets);
		ax_list_init(&grps_arr[grps_i].seq_list);

		// Parse group
		res = seq_read_group(grp_char, dif_c16(grp_char, fmt_char), &grps_arr[grps_i]);
		axcheck_b(res, axfree(grps_arr));

		grps_i++;
	}

	*grps = grps_arr;

	return AX_SUCC;
}

