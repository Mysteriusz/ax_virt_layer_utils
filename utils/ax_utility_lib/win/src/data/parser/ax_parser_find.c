#include "ax_parser.h"
#include "winreg.h"

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

	if (substr[0] == CHARSET_ANY[0]){
		return AX_SUCC;
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
			text_char -= dif_c16(substr, sub_char);
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

c16 parens_map[256] = {
	[L'('] = L')',
	[L'['] = L']',
	[L'{'] = L'}',
	[L'<'] = L'>',
	[L'"'] = L'"',
	[L'`'] = L'`',
	[L'\\'] = L'\\',
	[L'\''] = L'\'',
};
axres find_parentheses(
	_in const c16 		*text,
	_in c16			parens,
	_out_opt const c16	**loc // text location of the opposite parentheses character
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	c16 beg_char = parens;
	c16 end_char = parens_map[parens];

	// Invalid parentheses character
	if (end_char == 0){
		return AX_INV_DATA;
	}

	u64 text_len = _c16len(text);
	const c16 *text_char = text;

	bool found = false;
	i64 bal = 0;

	while(in_c16_s(text, text_char, text_len)){
		if (*text_char == beg_char){
			bal++;
		}else if (*text_char == end_char){
			bal--;
			found = bal == 0;
		}

		if (found){
			break;
		}

		text_char++;
	}

	if (!found){
		return AX_NOT_FND;
	}
	if (loc != nullptr){
		*loc = text_char; 
	}

	return AX_SUCC;
}

