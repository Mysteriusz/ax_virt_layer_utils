#include "ax_parser.h"

axres find_char(
	_in const c16 		*text,
	_in const c16 		val
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	const c16 *text_char = text;

	while(in_c16(text, text_char)
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

	const c16 *text_char = text;
	const c16 *sub_char = substr;

	const c16 *loc_start = substr;

	while(in_c16(text, text_char)
	&& in_c16(substr, sub_char)){
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

	bool part_found = (sub_char > substr); // Partial find
	bool found = (*sub_char == L'\0'); // Full find

	if (found){
		*loc = loc_start;
	}
	if (part_found){
		*sub_loc = sub_char;
	}

	return (found || part_found)
		? AX_SUCC
		: AX_NOT_FND;
}

