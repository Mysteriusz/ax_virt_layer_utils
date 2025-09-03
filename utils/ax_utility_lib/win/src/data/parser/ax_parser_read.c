#include "ax_parser.h"

axres read_until(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u32		*size,
	_in_out c16		*buf
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
	memcpy(buf, start, buf_size * sizeof(c16));

	return AX_SUCC;
}

axres read_line(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		*buf
){
	return read_until(text, CHARSET_NL, size, buf);
}
axres read_word(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		*buf
){
	return read_until(text, CHARSET_WS, size, buf);
}

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

	c16 *arg = nullptr;
	u32 buf_size = 0;
	u32 buf_offset = 0;
	for (u32 i = 0; i < n; i++){
		arg = va_arg(args, c16*);
		buf_size += _c16len(arg);

		if (!ret_size
		&& *size <= buf_size){
			return AX_BUF_TOO_SMALL;
		}

		if (!ret_size){
			memcpy(&buf[buf_offset], arg, _c16len_b(arg));
		}
		buf_offset += _c16len(arg);
	}
	va_end(args);
	
	// Add null-terminator
	buf_size++;
	if (ret_size){
		*size = buf_size;
		return AX_SUCC;
	}
	buf[buf_size - 1] = '\0';

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
	
	u32 buf_size = 0;
	u32 buf_ind = 0;
	bool fin = false;

	while(in_c16(text, start)){
		res = skip_until(start, charset, &end);
		u32 dif_size = dif_c16(start, end);

		if (res == AX_SUCC){
			buf_size = (dif_size > 0)
				? buf_size + 1
				: buf_size;
			end++;
			last = end;
		} else if(last != nullptr){
			buf_size++;
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
			t[dif_size] = L'\0';

			buf[buf_ind++] = t;
			
			if (fin){
				break;
			}
		}

		start = end;
	}

	if (ret_size){
		*size = buf_size; // Add one to compensate for the last skip.
		return AX_SUCC;
	}

	return AX_SUCC;
}
axres c_split_by(
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

