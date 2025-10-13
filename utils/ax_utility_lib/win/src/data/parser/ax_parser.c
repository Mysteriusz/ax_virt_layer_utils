#include "ax_parser.h"

axres starts_with(
	_in const c16		*text,
	_in const c16		*str,
	_out_opt const c16	**loc
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
		if (*text_char != *str_char){
			break;
		}

		text_char++;
		str_char++;
	}

	bool full_found = (*str_char == L'\0');
	bool part_found = !full_found && (*text_char == L'\0');

	bool found = full_found;

	if (loc != nullptr
	&& (part_found || full_found)){
		*loc = str_char;
		found = true; // Act as found if reading loc
	}

	// Got to the end of substr
	return (found == true)
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

	if (text[0] == UNICODE_ANY[0]){
		return AX_SUCC;
	}

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

axres contains_r(
	_in const c16 		*text,
	_in u64 		a,
	_in u64 		b,
	_in const c16		value	
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	const c16 *text_char = &text[a];
	u64 text_len = _c16len(text);

	if (text[0] == UNICODE_ANY[0]){
		return true;
	}

	u64 i = a;
	while(in_c16_s(text, text_char, text_len)
	&& i < b){
		if (*text_char == value){
			break;
		}
		text_char++;
		i++;
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
axres charset_count(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*count
){
	if (text == nullptr
	|| charset == nullptr){
		return AX_INV_ARG;
	}
	if (count == nullptr){
		return AX_INV_BUF;
	}

	u64 text_len = _c16len(text);
	const c16 *text_char = text;

	u64 occ = 0; 
	while(in_c16_s(text, text_char, text_len)){
		if (contains(charset, *text_char) == AX_SUCC){
			occ++;
		}
		text_char++;
	}

	*count = occ;

	return AX_SUCC;
}
axres reverse(
	_in const c16 		*text,
	_in_out c16 		*rev
){
	if (text == nullptr){
		return AX_INV_ARG;
	}
	if (rev == nullptr){
		return AX_INV_BUF;
	}

	u64 text_len = _c16len(text);
	memcpy(rev, text, text_len * sizeof(c16));

	c16 *text_end = &rev[text_len - 1];
	c16 *text_set = rev;
	c16 swap = L'\0';

	while(text_end != text_set){
		swap = *text_set;  
		*text_set = *text_end; 
		*text_end = swap;

		text_end--;
		text_set++;
	}


	return AX_SUCC;
}

axres c16_cat(
	_in const c16 		*a,
	_in const c16 		*b,
	_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (a == nullptr
	|| b == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	u64 a_len = _c16len(a);
	u64 b_len = _c16len(b);

	u64 buf_len = a_len + b_len + 1;
	if (ret_size){
		*size = buf_len;
		return AX_SUCC;
	}

	memcpy(buf, a, a_len * sizeof(c16)); 
	memcpy(buf + a_len, b, b_len * sizeof(c16)); 
	buf[buf_len - 1] = L'\0';

	return AX_SUCC;
}
axres c16_union(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u64 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (a == nullptr
	|| b == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	u64 a_len = _c16len(a);
	const c16 *a_char = a;
	
	u64 b_len = _c16len(b);
	const c16 *b_char = b;

	u64 buf_len = a_len + b_len;
	while(in_c16_s(b, b_char, b_len)){
		// If exist once remove it
		if (contains(a, *b_char) == AX_SUCC){
			buf_len--;
		}

		b_char++;
	}

	// Null-terminator
	buf_len++;
	if (ret_size){
		*size = buf_len;
		return AX_SUCC;
	}

	axcheck(_ax_buf_err(*size, buf_len));

	// Reset
	a_char = a;
	b_char = b;

	u64 buf_i = 0;
	// Write-back
	while(buf_i < (buf_len - 1)){
		if (contains(buf, *a_char) != AX_SUCC){
			buf[buf_i] = *a_char;
			a_char++;
		}else if (contains(buf, *b_char) != AX_SUCC){
			buf[buf_i] = *b_char;
			b_char++;
		}

		buf_i++;
	}

	return AX_SUCC;
}
axres c16_difference(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u64 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (a == nullptr
	|| b == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	u64 a_len = _c16len(a);
	const c16 *a_char = a;
	
	u64 b_len = _c16len(b);
	const c16 *b_char = b;

	u64 buf_len = a_len;
	while(in_c16_s(b, b_char, b_len)){
		// If exist once remove it
		if (contains(a, *b_char) == AX_SUCC){
			buf_len--;
		}

		b_char++;
	}

	// Null-terminator
	buf_len++;
	if (ret_size){
		*size = buf_len;
		return AX_SUCC;
	}

	axcheck(_ax_buf_err(*size, buf_len));

	// Reset
	a_char = a;
	b_char = b;

	u64 buf_i = 0;
	// Write-back
	while(buf_i < (buf_len - 1)){
		if (contains(b, *a_char) != AX_SUCC){
			buf[buf_i] = *a_char;
			buf_i++;
		}

		a_char++;
	}

	return AX_SUCC;
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

	u64 buf_len = text_len - count;
	if (ret_size){
		return buf_len;
	}

	memcpy(buf, beg, buf_len * sizeof(c16));

	return AX_SUCC;
}

