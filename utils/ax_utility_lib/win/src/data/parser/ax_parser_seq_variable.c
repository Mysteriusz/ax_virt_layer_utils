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
	res = read_until(label_char, JOIN_C16_C16(u';', UTF16_EOT), &name_len_n, name_buf);
	axcheck_r(res, nullptr);

	name_buf = axmalloc(name_len_n * sizeof(c16));

	res = read_until(label_char, JOIN_C16_C16(u';', UTF16_EOT), &name_len_n, name_buf);
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
	if (label_char == nullptr
	|| loc == nullptr){
		return type_unk;
	}

	enum var_type type = type_unk;

	axres res = AX_SUCC;

	u32 type_len_n = 0;
	c16 *type_buf = nullptr;
	
	// Read range until either u';' or to the end
	res = read_until(label_char, JOIN_C16_C16(u';', UTF16_EOT), &type_len_n, type_buf);
	axcheck_r(res, type_unk);

	type_buf = axmalloc(type_len_n * sizeof(c16));

	res = read_until(label_char, JOIN_C16_C16(u';', UTF16_EOT), &type_len_n, type_buf);
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
	if (label == nullptr){
		return true;
	}

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
	_in const c16		*label,
	_in u32 		spec_i
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
	u8 span = 1;

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
	axcheck_g(res, error_jump);

	var->spec_i = spec_i;
	var->name = name;
	var->type = type;
	var->length = length;
	var->span = span;

	return var;
	
error_jump:
	axfree(var);
	axfree(name);
	ax_log(res);

	return nullptr;
}
axres seq_group_var_end(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}

	const c16 *spec_char = fmt_char;
	u32 fmt_len = _c16len(fmt);

	while(in_c16_s(fmt, spec_char, fmt_len)){
		switch(*spec_char){
		case u']':
			if (!_is_esc(fmt, spec_char)){
				goto exit_jump;
			}
			break;
		default:
			break;
		}
		spec_char++;
	}
exit_jump:

	if (*spec_char != u']'){
		return AX_NOT_FND;
	}

	*loc = spec_char;

	return AX_SUCC;
}
axres seq_group_var(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*var_list,
	_in u32			spec_i,
	_out const c16		**loc
){
	if (fmt == nullptr
	|| fmt_char == nullptr
	|| var_list == nullptr){
		return AX_INV_ARG;
	}
	if (loc == nullptr){
		return AX_INV_BUF;
	}
	if (*fmt_char != u'['){
		return AX_INV_DATA;
	}

	axres res = AX_SUCC;

	const c16 *spec_char = nullptr;

	u32 spec_len_n = 0;
	c16 *spec_buf = nullptr;

	// Skip initial u'['
	fmt_char++;

	// Find ending of the variable label
	res = seq_group_var_end(fmt, fmt_char, &spec_char);
	axcheck(res);

	// Read inside of the variable label
	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len_n,
		spec_buf
	);
	axcheck(res);
	
	spec_buf = axmalloc(spec_len_n * sizeof(c16));

	res = read_range(
		fmt,
		dif_c16(fmt, fmt_char),
		dif_c16(fmt, spec_char),
		&spec_len_n,
		spec_buf
	);
	axcheck(res, axfree(spec_buf));

	fmt_var *var = _seq_label_to_var(spec_buf, spec_i);
	axfree(spec_buf);

	axcheck_r((var == nullptr), AX_INV_FMT);

	// Add to list
	var_list->add(
		var_list, 
		var,
		sizeof(fmt_var)
	);
	// Move to end + 1 since we want to skip last u']' character too
	fmt_char = spec_char + 1;

	*loc = fmt_char;

	axfree(var);

	return AX_SUCC;
}
axres seq_var_process(
	_in ax_list		*var_list,
	_in u32			match_i,
	_in c16			*match_res // Allocated result of the specifier at index spec_i
){
	if (var_list == nullptr){
		return AX_INV_ARG;
	}
	/*
	 	To ensure state validity accept this case
		as match_res is not guaranteed not to be nullptr from seq_action_nodet
	*/
	if (match_res == nullptr){
		return AX_SUCC;
	}

	// TODO: Save structure state for errors

	u32 cat_len_n = 0;
	c16 *cat_buf = nullptr;

	fmt_var *curr = nullptr;
	for (u32 i = 0; i < var_list->count; i++){
		curr = index_as(var_list, i, fmt_var*);
		asrt(curr != nullptr);
		io_i64(curr->spec_i);

		/*
		 	Collection switches
		*/
		if (curr->spec_i == match_i){
			curr->collect = true;
		}
		if (match_i == (curr->spec_i + curr->span)){
			curr->collect = false;
		}

		if (!curr->collect){
			continue;
		}

		// Create a buffer if doesnt exist
		if (curr->value == nullptr){
			curr->value = axmalloc(sizeof(c16));
			curr->value[0] = u'\0';
		}

		/*
			Concat curr->value with match_res
		*/
		c16_cat(
			curr->value,
			match_res,
			&cat_len_n,
			cat_buf
		);
		cat_buf = axmalloc(cat_len_n * sizeof(c16));
		c16_cat(
			curr->value,
			match_res,
			&cat_len_n,
			cat_buf
		);

		// Write-back
		axfree(curr->value);
		curr->value = cat_buf;
		io_str(curr->value);
	}

	return AX_SUCC;
}
