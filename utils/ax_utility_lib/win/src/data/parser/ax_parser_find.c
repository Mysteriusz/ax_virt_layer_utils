#include "ax_parser.h"

axres find_char(
	_in const c16 		*text,
	_in const c16 		val
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	u64 text_len = _c16len(text);
	const c16 *text_char = text;

	while(in_c16_s(text, text_char, text_len)
	&& *text_char != val){
		text_char++;
	}

	return (*text_char == val) 
		? AX_SUCC
		: AX_NOT_FND;
}

axres find_substr(
	_in const c16 		*text,
	_in const c16 		*substr,
	_out const c16		**loc,
	_out_opt const c16	**sub_loc 
){
	if (text == nullptr
	|| substr == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *loc_start = text;

	u64 text_len = _c16len(text);
	u64 sub_len = _c16len(substr);
	const c16 *text_char = text;
	const c16 *sub_char = substr;

	while(in_c16_s(text, text_char, text_len)
	&& in_c16_s(substr, sub_char, sub_len)){
		if (*sub_char == *text_char){
			if (loc_start == nullptr){
				loc_start = text_char;
			}
			sub_char++;

			if (*sub_char == L'\0'){
				break;
			}
		}else{
			loc_start = nullptr;
			sub_char = substr;	
		}

		text_char++;
	}

	bool full_find = (*sub_char == L'\0');
	bool part_find = ((sub_char > substr) && (*text_char == L'\0')); 
	bool find = full_find;

	if (sub_loc != nullptr
	&& (part_find || full_find)){
		*sub_loc = sub_char;
		find = true;
	}
	if (find){
		*loc = loc_start;
	}

	return (find)
		? AX_SUCC
		: AX_NOT_FND;
}

// Map specifier (%s, %i32, etc...) to its spec_meta in the table
const spec_meta *seq_spec_table_map(
	_in const c16		*spec
){
	if (spec == nullptr){
		return nullptr; 
	}

	const spec_meta *buf = nullptr;
	const c16 *temp = nullptr;

	axres res = AX_SUCC;
	for (u32 i = 0; i < SEQ_SPEC_TABLE_SIZE; i++){
		res = starts_with(spec, seq_spec_table[i].val, &temp);

		if (res == AX_SUCC){
			buf = &seq_spec_table[i];
			break;
		}
	}

	return buf;
}
axres seq_spec_load(
	_in const c16		*fmt,
	_out const spec_meta	***spec, // Specifier array
	_out const c16		***sep // Separator array
){
	if (fmt == nullptr){
		return AX_INV_ARG;
	}
	if (spec == nullptr
	|| sep == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	u64 fmt_len = _c16len(fmt);
	const c16 *fmt_char = fmt;

	u32 spec_count = 0;
	res = count(fmt, L"%", &spec_count);
	axcheck(res);

	const spec_meta **spec_buf = axmalloc(spec_count * sizeof(spec_meta*));
/*
	Separator count will always be one more than specifier

	Example:
 		- |%spec1|%spec2| - separator count == 3
		For each specifier there are 1 or 2 separators
*/
	const c16 **sep_buf = axmalloc((spec_count + 1) * sizeof(c16*));

	u64 sep_len = 0;
	u32 sep_i = 0;

	u32 spec_i = 0;

	while(in_c16_s(fmt, fmt_char, fmt_len)){
		// Read separator
		res = read_until(fmt_char, L"%", &sep_len, nullptr);
		axcheck_b(res);

		sep_buf[sep_i] = axmalloc(sep_len * sizeof(c16));

		res = read_until(fmt_char, L"%", &sep_len, (c16*)sep_buf[sep_i]);
		axcheck_b(res);

		fmt_char += (sep_len - 1);

		// Read specifier
		spec_buf[spec_i] = seq_spec_table_map(fmt_char);
		fmt_char += _c16len(spec_buf[spec_i]->val);

		spec_i++;
		sep_i++;
	}

	// Read last separator
	sep_len = _c16len(fmt_char);
	sep_buf[sep_i] = axmalloc((sep_len + 1) * sizeof(c16));
	memcpy((c16*)sep_buf[sep_i], fmt_char, sep_len * sizeof(c16));
	
	*spec = spec_buf;
	*sep = sep_buf;

	return AX_SUCC;
}
bool find_sequence_inv(
	_in const c16 		*fmt
){
	if (fmt == nullptr){
		return true;
	}
	if (fmt[0] == L'\0'){
		return true;
	}

	u64 fmt_len = _c16len(fmt); 

	const c16 *fmt_char = fmt; 
	const spec_meta *fmt_spec = nullptr;

	// Iterate specifiers
	while(in_c16_s(fmt, fmt_char, fmt_len)){
		// Find specifier`s metadata
		fmt_spec = seq_spec_table_map(fmt_char);

		if (*fmt_char == L'%'
		&& fmt_spec == nullptr){
			return true;
		}

		fmt_char += (fmt_spec != nullptr)
			? _c16len(fmt_spec->val)
			: 1;

		// If specifier needs separator but the next char is another specifier
		if (fmt_spec != nullptr
		&& fmt_spec->sep == true
		&& *fmt_char == L'%'){
			return true;
		}
	}

	return false;
}
axres find_sequence(
	_in const c16		*text,
	_in const c16 		*fmt
){
	if (find_sequence_inv(fmt)){
		return AX_INV_FMT;
	}
	if (text == nullptr){
		return AX_INV_ARG;
	}

	axres res = AX_SUCC;

	u64 text_len = _c16len(text);
	u64 fmt_len = _c16len(fmt);
	const c16 *text_char = text;
	const c16 *fmt_char = fmt;
	
	const spec_meta **spec_arr = nullptr;
	const c16 **sep_arr = nullptr;
	res = seq_spec_load(fmt, &spec_arr, &sep_arr);
	axcheck(res);

	// Loop until last format specifier and map
	while(in_c16_s(text, text_char, text_len)
	&& in_c16_s(fmt, fmt_char, fmt_len)){
		text_char++;
		fmt_char++;
	}

	io_str(sep_arr[0]);
	io_str(sep_arr[1]);
	io_str(sep_arr[2]);
	io_str(sep_arr[3]);

	return AX_SUCC;
}

