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

/*
 	Cast n field using part of the label.
*/
_free c16 *_seq_var_field_n(
	_in const c16 		*label_char,
	_out const c16		**loc
){
	if (label_char == nullptr
	|| loc == nullptr){
		return nullptr;
	}

	axres res = AX_SUCC;

	u32 name_len_n = 0;
	c16 *name_buf = nullptr;

	// Read range until either u';' or to the end
	res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &name_len_n, name_buf);
	axcheck_r(res, nullptr);

	name_buf = axmalloc(name_len_n * sizeof(c16));

	res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &name_len_n, name_buf);
	axcheck_r(res, nullptr);

	// Skip the text and write-back
	*loc = label_char + (name_len_n - 1);

	return name_buf;
}
/*
 	Cast t field using part of the label.
*/
enum var_type _seq_var_field_t(
	_in const c16 		*label_char,
	_out const c16		**loc
){
	enum var_type type = type_unk;

	axres res = AX_SUCC;

	u32 type_len_n = 0;
	c16 *type_buf = nullptr;
	
	// Read range until either u';' or to the end
	res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &type_len_n, type_buf);
	axcheck_r(res, type_unk);

	type_buf = axmalloc(type_len_n * sizeof(c16));

	res = read_until(label_char, JOIN_C16(u';', UTF16_EOT), &type_len_n, type_buf);
	axcheck_r(res, type_unk, axfree(type_buf));

	if (compare(type_buf, u"u8") == AX_SUCC){
		label_char += _c16len(u"u8");
		type = type_u8;	
	}else if (compare(type_buf, u"u16") == AX_SUCC){
		label_char += _c16len(u"u16");
		type = type_u16;	
	}else if (compare(type_buf, u"u32") == AX_SUCC){
		label_char += _c16len(u"u32");
		type = type_u32;
	}else if (compare(type_buf, u"u64") == AX_SUCC){
		label_char += _c16len(u"u64");
		type = type_u64;
	}
	axfree(type_buf);

	// Skip the text and write-back
	*loc = label_char;

	return type;
}

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
			// Try to parse the field to validate value
			if (_seq_var_field_t((label_char + 1), &label_char) == type_unk){
				return true;
			}
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
	c16 *name = nullptr;
	enum var_type type = type_unk;
	u32 length = 0;
	u8 span = 0;

	/*
	 	Process one field per loop.
	*/
	while(in_c16_s(label, label_char, label_len)){
		switch(*label_char){
		case u'n':
			// Skip the field head. ex: u"n:"
			label_char++;
			label_char++;

			name = _seq_var_field_n(label_char, &label_char);
			axcheck_g((name == nullptr), error_jump);

			break;
		case u't':
			// Skip the field head. u"t:"
			label_char++;
			label_char++;

			type = _seq_var_field_t(label_char, &label_char);
			axcheck_g((type == type_unk), error_jump);

			break;
		/*
			Impossible case in which field character is not known (invalidated by seq_label_to_var_inv)
		*/
		default:
			goto error_jump;
		}

		label_char++;
	}
	axcheck_r(res, nullptr, axfree(var));

	var->name = name;
	var->type = type;
	var->length = length;
	var->span = span;
	io_str(var->name);
	io_i64(var->type);

	return var;
	
error_jump:
	axfree(var);
	axfree(name);
	ax_log(res);

	return nullptr;
}
