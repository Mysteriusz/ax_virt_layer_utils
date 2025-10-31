#include "ax_parser_seq.h"

#define field_check(v) \
	if (in_field == false){ \
		/* If has already occured */ \
		if ((v) == true){ \
		return true; \
		} \
		(v) = true; \
		label_char++; \
		if (*label_char != u':'){ \
			return true; \
		} \
		in_field = true; \
	} \

bool seq_label_to_var_inv(
	_in const c16		*label
){
	u32 label_len = _c16len(label);
	const c16 *label_char = label;

	bool in_field = false; 
	bool n_field = false; // n_field occurence
	bool t_field = false; // t_field occurence

	while(in_c16_s(label, label_char, label_len)){
		switch(*label_char){
		/*
		 	Field characters
			TODO: accept other fields
		*/
		/*case u's':
		case u'l':*/
		case u't':
			field_check(t_field);
			break;
		case u'n':
			field_check(n_field);
			break;
		/*
			Misc.
		*/
		case u';':
			if (in_field == false){
				return true;
			}
			if(!_is_esc(label, label_char)){
				in_field = false;
			}
			break;
		default:
			if (in_field == false){
				return true;
			}
			break;
		}
		label_char++;
	}

	/* 
	 	Field without explicit ending 
	 	Example:
			- u"n:key;t"
	*/
	if (*label_char != u':'){
		in_field = true;
	}

	return (in_field == false)
		? true
		: false;
}

_free fmt_var *_seq_label_to_var(
	_in const c16		*label
){
	if (seq_label_to_var_inv(label)){
		return nullptr;
	}

	axres res = AX_SUCC;

	u32 label_len = _c16len(label);
	const c16 *label_char = label;

	fmt_var *var = axmalloc(sizeof(fmt_var));

	// Variable field buffers
	c16 *name_buf = nullptr;
	u32 name_len_n = 0;

	/*
	 	Process one field per loop.
	*/
	while(in_c16_s(label, label_char, label_len)){
		switch(*label_char){
		case u'n':{
			// Skip the field head. ex: u'n:"
			label_char++;
			label_char++;

			// Read range until either u';' or to the end
			res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &name_len_n, name_buf);
			axcheck_g(res, error_jump);

			name_buf = axmalloc(name_len_n * sizeof(c16));

			res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &name_len_n, name_buf);
			axcheck_g(res, error_jump);

			label_char += name_len_n;
			break;
		}
		default: // Impossible case (invalidated by seq_label_to_var_inv)
			break;
		}

		label_char++;
	}
	axcheck_r(res, nullptr, axfree(var));

	return var;
	
error_jump:
	axfree(var);
	axfree(name_buf);
	ax_log(res);

	return nullptr;
}
