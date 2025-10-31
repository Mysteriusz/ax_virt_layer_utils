#include "ax_parser.h"

axres read_until(
	_in const c16 		*text,
	_in const c16		*charset,
	_in_out u32		*size,
	_in_out _eval c16	*buf
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
	
	axres res = AX_SUCC;

	const c16 *start = text;
	const c16 *end = nullptr;

	// Read to end WHEN charset == u""
	if (_c16len(charset) == 0){
		end = &start[_c16len(start)];
	}else{
 		res = skip_until(start, charset, &end);
		axcheck(res);
	}
	 
	u64 buf_len_n = dif_c16(start, end) + 1;
	if (ret_size){
		*size = buf_len_n;
		return AX_SUCC;
	}

	// Validate provided buffer size
	axcheck(_ax_buf_err(*size, buf_len_n));

	// Adjust size to copy without null-terminator
	memcpy(buf, start, (buf_len_n - 1) * sizeof(c16));

	return AX_SUCC;
}

axres read_range(
	_in const c16 		*text,
	_in u32			from,
	_in u32			to,	
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
){
	if (text == nullptr){ 
		return AX_INV_ARG;
	}

	u32 text_len = _c16len(text);
	if (to <= from
	|| text_len < from // from index check
	|| text_len < to){ // to index check
		return AX_INV_IND;
	}
	
	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}
	
	u64 buf_len_n = (to - from) + 1;
	if (ret_size){
		*size = buf_len_n;
		return AX_SUCC;
	}

	// Validate provided buffer size
	axcheck(_ax_buf_err(*size, buf_len_n));

	// Adjust size to copy without null-terminator
	memcpy(buf, &text[from], (buf_len_n - 1) * sizeof(c16));

	return AX_SUCC;
}

axres read_line(
	_in const c16 		*text,
	_in_out u32		*size,
	_in_out _eval c16	*buf
){
	return read_until(text, CHARSET_NL, size, buf);
}
axres read_word(
	_in const c16 		*text,
	_in_out u32		*size,
	_in_out _eval c16	*buf
){
	return read_until(text, CHARSET_WS, size, buf);
}

#include <stdarg.h>
axres join_with(
	_in _eval c16		*buf,
	_in_out u32		*size,
	_in u32 		n,
	...
){
	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

	va_list args;
	va_start(args, n);

	// TODO: INVAuIDATION OF THE *size

	c16 *arg = nullptr;
	u32 buf_len_n = 1;
	u32 buf_offset = 0;
	for (u32 i = 0; i < n; i++){
		arg = va_arg(args, c16*);
		buf_len_n += _c16len(arg);

		if (!ret_size
		&& *size <= buf_len_n){
			return AX_BUF_TOO_SMALL;
		}

		if (!ret_size){
			memcpy(&buf[buf_offset], arg, _c16len_b(arg));
		}
		buf_offset += _c16len(arg);
	}

	va_end(args);
	
	if (ret_size){
		*size = buf_len_n;
		return AX_SUCC;
	}
	buf[buf_len_n - 1] = '\0';

	return AX_SUCC;
}

axres split_by(
	_in const c16		*text,
	_in const c16		*charset,
	_out u32		*size, // Evaluate by using (size * sizeof(c16*))
	_in_out c16		**buf
){
	// All arguments that cannot be nullptr.
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

	axres res = AX_SUCC;

	const c16 *start = text;
	const c16 *end = nullptr;
	const c16 *last = nullptr;
	
	u32 buf_len_n = 1;
	u32 buf_ind = 0;
	bool fin = false;

	while(in_c16(text, start)){
		res = skip_until(start, charset, &end);
		u64 dif_size = dif_c16(start, end);

		if (res == AX_SUCC){
			buf_len_n = (dif_size > 0)
				? buf_len_n + 1
				: buf_len_n;
			end++;
			last = end;
		} else if(last != nullptr){
			buf_len_n++;
			if (ret_size){
				break;
			}

			dif_size = _c16len(last);
			start = last;
			fin = true;
		}

		if (dif_size > 0
		&& !ret_size){
			c16 *t = axmalloc((dif_size + 1) * sizeof(c16));
			memcpy(t, start, dif_size * sizeof(c16)); 
			t[dif_size] = u'\0';

			buf[buf_ind++] = t;
			
			if (fin){
				break;
			}
		}

		start = end;
	}

	if (ret_size){
		*size = buf_len_n;
		return AX_SUCC;
	}

	return AX_SUCC;
}
axres split_by_c(
	_in c16			**buf,
	_in u32			size
){
	if (buf == nullptr){
		return AX_INV_ARG;
	}

	for (u32 i = 0; i < size; i++){
		axfree((void*)buf[i]);
	}

	return AX_SUCC;
}

