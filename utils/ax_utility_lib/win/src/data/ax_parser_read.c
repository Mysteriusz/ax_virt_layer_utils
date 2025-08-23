#include "ax_parser.h"
#include <string.h>

axres read_until(
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
	
	axres res = AX_SUCC;

	const c16 *start = text;
	const c16 *end = nullptr;

 	res = skip_until(start, charset, &end);
	if (AX_ERR(res)){
		return res;
	}
	 
	u32 buf_size = dif_c16(start, end); 
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

axres read_line(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		**buf
){
	return read_until(text, CHARSET_NL, size, buf);
}
axres read_word(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		**buf
){
	return read_until(text, CHARSET_WS, size, buf);
}

