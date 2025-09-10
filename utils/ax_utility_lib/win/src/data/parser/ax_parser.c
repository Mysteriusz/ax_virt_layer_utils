#include "ax_parser.h"

axres starts_with(
	_in const c16		*text,
	_in const c16		*str
){
	if (text == nullptr
	|| str == nullptr){
		return AX_INV_ARG;
	}

	const c16 *text_char = text;
	const c16 *str_char = str;

	u64 text_len = _c16len(text);
	u64 str_len = _c16len(str);

	while(in_c16_s(text, text_char, text_len)
	&& in_c16_s(str, str_char, str_len)){
		text_char++;
		str_char++;

		if (*text_char != *str_char){
			break;
		}
	}
	
	// Got to the end of substr
	return (*str_char == L'\0')
		? AX_SUCC
		: AX_NOT_FND;
}

axres contains(
	_in const c16 		*text,
	_in const c16		value	
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	const c16 *text_char = text;
	u64 text_len = _c16len(text);

	while(in_c16_s(text, text_char, text_len)){
		if (*text_char == value){
			break;
		}
		text_char++;
	}

	return (*text_char == value)
		? AX_SUCC
		: AX_NOT_FND;
}

axres compare(
	_in const c16 		*a,
	_in const c16		*b	
){
	if (a == nullptr
	|| b == nullptr){
		return AX_INV_ARG;
	}

	while(*a != L'\0'
	&& *b != L'\0'
	&& *a == *b){
		a++;
		b++;
	}

	return (*a == *b)
		? AX_SUCC
		: AX_NOT_FND;
}

axres trim(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}
	
	u64 text_len = _c16len(text);

	u64 count = 0; // Trim count
	u8 bal = 0; // Trim balance

	const c16 *beg = text;
	const c16 *end = &text[text_len - 1];

	while(count < text_len
	&& in_c16_s(text, beg, text_len)
	&& in_c16_s(text, end, text_len)){
		bal = 0;
		if (contains(charset, *beg) == AX_SUCC){
			beg++; count++; bal++;
		}
		if (contains(charset, *end) == AX_SUCC){
			end--; count++; bal++;
		}
		// None trimmed if balance is 0
		if (bal == 0){
			break;
		}
	}

	u64 buf_size = text_len - count;
	if (ret_size){
		return buf_size;
	}

	memcpy(buf, beg, buf_size * sizeof(c16));

	return AX_SUCC;
}

