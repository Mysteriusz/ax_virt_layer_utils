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

	while(in_c16(text,text_char)
	&& in_c16(str,str_char)
	&& *text_char == *str_char){
		text_char++;
		str_char++;
	}
	
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

	while(in_c16(text, text_char)){
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
	_out u32		*size,
	_in_out c16		**buf
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		&& buf == nullptr){
			return AX_INV_BUF;
		}
	}
	
	const c16 *start = text;
	const c16 *end = end_c16(text);
	
	while((contains(charset, *start) == AX_SUCC)
	&& end != start){
		start++;
	}

	while((contains(charset, *end) == AX_SUCC)
	&& end != start){
		end--;
	}

	if (end == start){
		return AX_INV_DATA;
	}

	u32 buf_size = dif_c16(start, ++end); 
	if (ret_size == true){
		*size = buf_size;
		return AX_SUCC;
	}

	if (buf_size == 0){
		return AX_INV_DATA;
	}
	memcpy(*buf, start, buf_size * sizeof(c16)); 

	return AX_SUCC;
}

