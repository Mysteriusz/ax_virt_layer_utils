#include "ax_parser.h"
#include "stdbool.h"

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

	u32 text_len = _c16len(text);
	const c16 *text_char = text;

	bool found = false;

	while(in_c16_s(text, text_char, text_len)){
		found = (contains(charset,*text_char) == AX_SUCC);

		if (found == true){
			break;
		}else{
			text_char++;
		}
	}

	if (found == true
	|| contains(charset, UTF16_EOT) == AX_SUCC){
		*loc = text_char;
		found = true;
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

	u32 text_len = _c16len(text);
	const c16 *text_char = text;

	u32 occ_n = 0;

	bool found = false;

	do{
		if (contains(charset,*text_char) == AX_SUCC){
			found = (occ_n == n);
			occ_n++;
		}

		if (found == true){
			break;
		}else {
			text_char++;
		}
	}while(in_c16_s(text, text_char, text_len));

	if (found == true){
		*loc = text_char;
	}

	return (found == true)
		? AX_SUCC
		: AX_NOT_FND;
}
axres skip_until_r(
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
	
	u32 text_len = _c16len(text);
	const c16 *text_char = &text[text_len - 1];

	bool found = false;

	while(in_c16_s(text, text_char, text_len)){
		found = (contains(charset,*text_char) == AX_SUCC);

		if (found == true){
			break;
		}else{
			text_char--;
		}
	}

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

	u32 text_len = _c16len(text);
	u32 word_len = _c16len(word);
	const c16 *text_char = text;
	const c16 *word_char = word;

	bool found = false;

	while(in_c16_s(text, text_char, text_len)
	&& in_c16_s(word, word_char, word_len)){
		text_char++;
		word_char++;
	}

	found = (*word_char == u'\0');
	if (found){
		*loc = text_char;
	}

	return (found == true)
		? AX_SUCC
		: AX_NOT_FND;
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

	u32 text_len = _c16len(text);
	const c16 *text_char = text;

	while (in_c16_s(text, text_char, text_len)){
		if (contains(charset, *text_char) == AX_SUCC){
			text_char++;
		}
		else{
			break;
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

	res = skip_until(text, CHARSET_NL UTF16_EOT_STR, &temp);
	axcheck(res);

	// Skip depending on EOL encoding
	if (is_crlf(temp)){
		temp += 2;
	}else if(is_lf(temp)){
		temp++;
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
	axcheck(res);

	temp++;	// skip CHARSET_NL character
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
	axcheck(res);

	*loc = temp;

	return AX_SUCC;
}

