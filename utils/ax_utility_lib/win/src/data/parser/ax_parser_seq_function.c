/*
 	Sequence finder function interface
*/

#include "ax_parser.h"

bool seq_func_to_cond_inv(
	_in const c16		*func // func for function
){
	if (func == nullptr){
		return true;
	}

	u64 func_len = _c16len(func);
	const c16 *func_char = func;

	// Check function bound characters
	if (*func_char != L'('){
		return true;
	}
	if (func[func_len - 1] != L')'){
		return true;
	}

	func_char++;
	if (in_c16_s(func, func_char, func_len) == false){
		return true;
	}

	// Get function return mode
	switch(*func_char){
	case L'!': // Not true mode
	case L'+': // True mode
		break;
	default: // Unknown mode value
		return true;
	}
	func_char++;

	if (in_c16_s(func, func_char, func_len) == false
	|| *func_char != L':'){
		return true;
	}

	func_char++;
	if (in_c16_s(func, func_char, func_len) == false){
		return true;
	}

	// Check condition bound characters
	if (*func_char != L'`'){
		return true;
	}
	if (func[func_len - 2] != L'`'){
		return true;
	}

	// Count any characters
	u64 c = 0;
	charset_count(func_char, L"$", &c);
	if (c == 0){
		return true;
	}

	return false;
}
_free fmt_cond *seq_func_to_cond(
	_in const c16		*func // func for function
){
	if (seq_func_to_cond_inv(func)){
		return nullptr;
	}

	axres res = AX_SUCC;

	u64 func_len = _c16len(func);
	const c16 *func_char = func;

	fmt_cond *cond = axmalloc(sizeof(fmt_cond));

	enum cond_mode mode = 0;

	c16 *bef_buf = nullptr;
	u64 bef_len = 0;

	c16 *aft_buf = nullptr;
	u64 aft_len = 0;

	// Skip first '('
	func_char++; 

	// Read return type
	bool ret = (*func_char == L'!') 
		? false
		: true;

	// Skip mode char '!' or '+'
	func_char++;

	// Skip forwarding char ':`'
	func_char += 2;

	// Read the before string
	res = read_until(func_char, SEQ_COND_CHARSET, &bef_len, bef_buf);
	axcheck_g(res, cleanup);

	// Finish the read if there are characters between func_char and SEQ_COND_CHARSET
	if (bef_len > 1){
		bef_buf = axmalloc(bef_len * sizeof(c16));

		res = read_until(func_char, SEQ_COND_CHARSET, &bef_len, bef_buf);
		axcheck_g(res, cleanup);

		mode |= condition_bef;
	}

	// Skip bef_buf
	func_char += bef_len;

	// Read the after string
	res = read_range(
		func,
		dif_c16(func, func_char),
		func_len - 2, // One char before L"`"
		&aft_len,
		aft_buf
	);
	if (AX_ERR(res) == false){
		aft_buf = axmalloc(aft_len * sizeof(c16));

		res = read_range(
			func,
			dif_c16(func, func_char),
			func_len - 2, // One char before L"`"
			&aft_len,
			aft_buf
		);
		axcheck_g(res, cleanup);
		mode |= condition_aft;
	}
	
	// Read the after string
	cond->bef = bef_buf;
	cond->aft = aft_buf;
	cond->ret = ret;
	cond->mode = mode;

	return cond;

cleanup:
	axfree(aft_buf);
	axfree(bef_buf);
	return nullptr;
}

axres seq_group_conditions(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*cond_list,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr
	|| cond_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}
	if (*fmt_char != L'('){
		return AX_INV_FMT;
	}

	axres res = AX_SUCC;

	const c16 *spec_char = nullptr;

	u64 spec_len = 0;
	c16 *spec_buf = nullptr;

	// Find ending of the function
	res = find_substr(fmt_char, L"`)", &spec_char, nullptr);
	axcheck(res);

	// Read inside of the function
	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char + 2),
		&spec_len,
		spec_buf
	);
	axcheck(res);
	
	spec_buf = axmalloc(spec_len * sizeof(c16));

	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char + 2),
		&spec_len,
		spec_buf
	);
	axcheck(res, axfree(spec_buf));

	fmt_cond* cond = seq_func_to_cond(spec_buf);
	axcheck_r((cond == nullptr), AX_INV_FMT, axfree(spec_buf));

	cond_list->add(
		cond_list,
		cond,
		sizeof(fmt_cond)
	);
	fmt_char += (spec_len - 1);

	// Cleanup
	axfree(cond);
	axfree(spec_buf);

	// Write-back
	*loc = fmt_char;

	return AX_SUCC;
}
axres seq_conditions_match(
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

	// TODO: Condition checking logic update
	axres res = AX_SUCC;
	unref(res);
	const c16 *text_char = text;

	fmt_cond *curr = nullptr;
	unref(curr);
	for (u64 i = 0; i < cond_list->count; i++){
		curr = index_as(cond_list, i, fmt_cond*);
	}

	*loc = text_char;

	return AX_SUCC;
}

