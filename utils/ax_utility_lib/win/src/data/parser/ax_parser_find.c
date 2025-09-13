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

bool find_sequence_spec_inv(
	_in const c16		*spec,
	_out const spec_meta	**meta
){
	if (spec == nullptr
	|| spec[0] != L'%'){
		return true;
	}
	if (meta == nullptr){
		return true;
	}

	axres res = AX_SUCC;
	for (u32 i = 0; i < SEQ_SPC_TABLE_SIZE; i++){
		res = starts_with(spec, seq_spec_table[i].val, &spec);

		if (res == AX_SUCC){
			*meta = &seq_spec_table[i];
			break;
		}
	}

	if (AX_ERR(res)){
		return true;
	}

	return false;
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
		// Check specifier and read it`s metadata
		if (*fmt_char == L'%'
		&& find_sequence_spec_inv(fmt_char, &fmt_spec)){
			return true;
		} else if(*fmt_char != L'%'){
			fmt_spec = nullptr;
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

	return AX_SUCC;
}

