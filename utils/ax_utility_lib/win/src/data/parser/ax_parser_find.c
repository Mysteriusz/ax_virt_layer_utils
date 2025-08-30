#include "ax_parser.h"

axres find_char(
	_in const c16 		*text,
	_in const c16 		val
){
	if (text == nullptr){
		return AX_INV_ARG;
	}

	const c16 *text_char = text;

	while(in_c16(text, text_char)
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
	_out const c16		**loc 
){
	if (text == nullptr
	|| substr == nullptr){
		return AX_INV_ARG;
	}

	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *text_char = text;

	bool found = false;
	while(in_c16(text,text_char)
	&& found == false){
		found = (starts_with(text_char,substr) == AX_SUCC)
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

axres find_eval_node(
	_in u32			n,
	_in struct eval_node	*nodes,
	_out c16		**ret
){
	if (nodes == nullptr){
		return AX_INV_ARG; 
	}
	if (ret == nullptr){
		return AX_INV_BUF; 
	}

	struct eval_node curr = nodes[0];
	for (u32 i = 0; i < n; i++){
		if (curr.func == true){
			*ret = curr.ret;
			break;
		}
		curr = nodes[i];
	}
	if (*ret == nullptr){
		return AX_NOT_FND;
	}

	return AX_SUCC;
}

