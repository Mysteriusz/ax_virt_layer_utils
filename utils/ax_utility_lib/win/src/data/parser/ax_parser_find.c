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

	bool found = (*sub_char == L'\0') // Full find
		|| ((sub_char > substr) && (*text_char == L'\0')); // Part find

	if (found){
		*loc = loc_start;
		if (sub_loc != nullptr){
			*sub_loc = sub_char;
		}
	}

	return (found)
		? AX_SUCC
		: AX_NOT_FND;
}

