#include "ax_parser.h"

axres c16_to_int(
	_in const c16		*text,
	_out iu64 		*value
){
	if (text == nullptr){
		return AX_INV_ARG;
	}
	if (value == nullptr){
		return AX_INV_BUF;
	}

	i64 i = 0;
	i8 sign = 1;
	
	if (text[0] == u'-'){
		sign = -1;
		text = &text[1];
	}

	for (u32 j = 0; j < _c16len(text); j++){
		if ((u'0' <= text[j] && text[j] <= u'9') == false){
			return AX_INV_DATA;
		}
		i = 10 * i + (c16)text[j] - (c16)u'0';
	}
	if (sign){
		value->unsig_64 = sign * i;
	}else{
		value->sig_64 = sign * i;
	}

	return AX_SUCC;
}

