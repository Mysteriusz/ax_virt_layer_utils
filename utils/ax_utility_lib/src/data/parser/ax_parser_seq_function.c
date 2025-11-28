/*
 	Sequence finder function interface
*/

#include "ax_parser_seq.h"

bool seq_func_to_cond_inv(
	_in const c16		*func // func for function
){
	if (func == nullptr){
		return true;
	}

	u32 func_len = _c16len(func);
	const c16 *func_char = func;

	// Get function return mode
	switch(*func_char){
	case u'!': // Not true mode
	case u'+': // True mode
		break;
	default: // Unknown mode value
		return true;
	}
	func_char++;

	if (in_c16_s(func, func_char, func_len) == false
	|| *func_char != u':'){
		return true;
	}

	func_char++;
	if (in_c16_s(func, func_char, func_len) == false){
		return true;
	}

	// Count any characters
	u32 c = 0;
	charset_count(func_char, SEQ_COND_CHARSET, &c);
	if (c == 0){
		return true;
	}

	return false;
}
_free fmt_cond *_seq_func_to_cond(
	_in const c16		*func // func for function
){
	if (seq_func_to_cond_inv(func)){
		return nullptr;
	}

	axres res = AX_SUCC;

	u32 func_len = _c16len(func);
	const c16 *func_char = func;

	fmt_cond *cond = axmalloc(sizeof(fmt_cond));

	c16 *bef_buf = nullptr;
	u32 bef_len = 0;

	c16 *aft_buf = nullptr;
	u32 aft_len = 0;

	// Read return type
	bool ret = (*func_char == u'!') 
		? false
		: true;

	// Skip mode char '!' or '+'
	func_char++;

	// Skip forwarding char ':`
	func_char++;

	// Read the before string
	res = read_until(func_char, SEQ_COND_CHARSET, &bef_len, bef_buf);
	axcheck_g(res, cleanup);

	// Finish the read if there are characters between func_char and SEQ_COND_CHARSET
	if (bef_len > 1){
		bef_buf = axmalloc(bef_len * sizeof(c16));

		res = read_until(func_char, SEQ_COND_CHARSET, &bef_len, bef_buf);
		axcheck_g(res, cleanup);
	}

	// Skip bef_buf
	func_char += bef_len;

	// Read the after string
	res = read_range(
		func,
		dif_c16(func, func_char),
		func_len,
		&aft_len,
		aft_buf
	);

	if (AX_ERR(res) == false){
		aft_buf = axmalloc(aft_len * sizeof(c16));

		res = read_range(
			func,
			dif_c16(func, func_char),
			func_len,
			&aft_len,
			aft_buf
		);
		axcheck_g(res, cleanup);
	}
	
	// Read the after string
	cond->bef = bef_buf;
	cond->aft = aft_buf;
	cond->ret = ret;
	cond->state = outside;

	return cond;

cleanup:
	axfree(aft_buf);
	axfree(bef_buf);
	return nullptr;
}

axres seq_group_cond_end(
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

	const c16 *spec_char = fmt_char;
	u32 fmt_len = _c16len(fmt);

	while(in_c16_s(fmt, spec_char, fmt_len)){
		switch(*spec_char){
		case u')':
			if (!_is_esc(fmt, spec_char)){
				goto exit_jump;
			}
			break;
		default:
			break;
		}
		spec_char++;
	}
exit_jump:

	if (*spec_char != u')'){
		return AX_NOT_FND;
	}

	*loc = spec_char;
	return AX_SUCC;
}
axres seq_group_cond(
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
	if (*fmt_char != u'('){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	const c16 *spec_char = nullptr;

	u32 spec_len_n = 0;
	c16 *spec_buf = nullptr;

	// Skip initial u'('
	fmt_char++;

	// Find ending of the function
	res = seq_group_cond_end(fmt, fmt_char, &spec_char);
	axcheck(res);

	// Read inside of the function
	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len_n,
		spec_buf
	);
	axcheck(res);
	
	spec_buf = axmalloc(spec_len_n * sizeof(c16));

	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len_n,
		spec_buf
	);
	axcheck(res, axfree(spec_buf));

	// Parse condition with read block
	fmt_cond *cond = _seq_func_to_cond(spec_buf);
	axfree(spec_buf);

	axcheck_r((cond == nullptr), AX_INV_FMT);

	// Add to list
	cond_list->add(
		cond_list, 
		cond,
		sizeof(fmt_cond)
	);
	// Move to end + 1 since we want to skip last u')' character too
	fmt_char = spec_char + 1;

	*loc = fmt_char;

	axfree(cond);

	return AX_SUCC;
}

axres seq_condition_match(
	_in const c16 		*text,
	_in const c16		*text_char,
	_in fmt_cond 		*cond,
	_out const c16		**loc
){
	if(text == nullptr
	|| text_char == nullptr
	|| cond == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *spec_char = text_char;

	// Outside state and current text_char is a begining of the conditioned sector
	if (cond->bef != nullptr
	&& starts_with(text_char, cond->bef, nullptr) == AX_SUCC
	&& cond->state == outside){
		cond->state = inside;
		spec_char += _c16len(cond->bef);
	}
	// Inside state and current text_char is an ending of the conditioned sector
	else if (cond->aft != nullptr
	&& starts_with(text_char, cond->aft, nullptr) == AX_SUCC
	&& cond->state == inside){
		cond->state = outside;
		//find_substr(text_char, cond->aft, &spec_char, nullptr);
		spec_char += _c16len(cond->aft);
	}

	// Return based on the return mode of the condition
	if(cond->state == inside
	&& cond->ret == false){
		return AX_NOT_FND;
	}
	if (cond->state == outside
	&& cond->ret == true){
		return AX_NOT_FND;
	}

	*loc = spec_char;

	return AX_SUCC;
}

