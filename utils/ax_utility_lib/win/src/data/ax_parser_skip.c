#include "ax_parser.h"

axres skip_until(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}

	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;

	bool found = false;

	while(in_c16(text,text_char)
	&& found == false){
		found = (contains(charset,*text_char) == AX_SUCC)
			? true
			: false;

		if (found == false){
			text_char++;
		}
	}

	if (found == true){
		*loc = text_char;
	}

	return (found == true)
		? AX_SUCC
		: AX_NOT_FND;
}

axres skip_until_n(
	_in const c16		*text,
	_in const c16		*charset,
	_in u32			n,
	_out const c16		**loc
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}

	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;
	u32 o = 0;

	bool found = (o == n);

	do{
		o = (contains(charset,*text_char) == AX_SUCC)
			? o + 1
			: o;
		found = (o - 1 == n);

		if (found == false){
			text_char++;
		}
		
	}while(in_c16(text,text_char)
	&& found == false);

	if (found == true){
		*loc = text_char;
	}

	return (found == true)
		? AX_SUCC
		: AX_NOT_FND;
}

axres skip_word(
	_in const c16 		*text,
	_in c16			*word,
	_out const c16		**loc
){
	if (text == nullptr
	|| word == nullptr){
		return AX_INV_ARG; 
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;
	const c16 *word_char = word;

	bool found = false;

	while(in_c16(text, text_char)
	&& in_c16(word, word_char)
	&& *text_char == *word_char){
		text_char++;
		word_char++;
	}

	found = (*word_char == L'\0');
	if (!found){
		return AX_NOT_FND;	
	}
	if (*text_char == L'\0'){
		return AX_INV_DATA;
	}

	*loc = text_char;

	return AX_SUCC;
}

axres skip_while(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}

	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;

	bool found = true;

	while (in_c16(text,text_char)
	&& found == true){
		found = (contains(charset, *text_char) == AX_SUCC)
			? true
			: false;

		if (found == true){
			text_char++;
		}
	}

	*loc = text_char;

	return AX_SUCC;
}

axres skip_line(
	_in const c16		*text,
	_out const c16		**loc
){
	if (text == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	const c16 *temp = nullptr;

	res = skip_until(text, CHARSET_NL, &temp);
	if (AX_ERR(res)){
		return res;
	}

	temp++;	
	if (*temp == L'\0'){
		return AX_NOT_FND;
	}

	*loc = temp;
	
	return AX_SUCC;
}
axres skip_line_n(
	_in const c16		*text,
	_in u32			n,
	_out const c16		**loc
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	const c16 *temp = nullptr;

	res = skip_until_n(text, CHARSET_NL, n, &temp);
	if (AX_ERR(res)){
		return res;
	}

	temp++;	
	if (*temp == L'\0'){
		return AX_NOT_FND;
	}

	*loc = temp;
	
	return AX_SUCC;
}

axres skip_ws(
	_in const c16		*text,
	_out const c16		**loc
){
	if (text == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;
	const c16 *temp = nullptr;

	res = skip_while(text, CHARSET_WS, &temp);
	if (AX_ERR(res)){
		return res;
	}

	*loc = temp;

	return AX_SUCC;
}

