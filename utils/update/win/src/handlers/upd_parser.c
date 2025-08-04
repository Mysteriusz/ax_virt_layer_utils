#include "upd_parser.h"

AXSTATUS upd_command_parse(
	AX_IN const wchar_t** 	 	args,
	AX_IN const uint32_t		arg_count,
	AX_OUT UPD_COMMAND**		command
){
	return AX_SUCCESS;
}

AXSTATUS upd_token_parse(
	AX_IN const wchar_t*	 	value,
	AX_IN const uint32_t		value_length,
	AX_OUT UPD_COMMAND_TOKEN**	token
){
	if (value == NULL || 
		token == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* current = value;
	const wchar_t* skip = NULL;

	// Skip all leading white spaces
	skip = upd_skip(current, UPD_EMPTY_SKIP_SET, UPD_SKIP_FLAG_ALL);
	if (skip != NULL) current = skip;
		
	void* token_value = NULL;
	size_t token_value_size = 0;
	UPD_COMMAND_TOKEN_TYPE token_type = SWITCH; 

	// Skip all leading token entry characters
	skip = upd_skip(current, UPD_TOKEN_START_SKIP_SET, UPD_SKIP_FLAG_ALL);
	if (skip != NULL) current = skip;

	// Skip first expression character (if found)
	const wchar_t* exp_skip = upd_skip(current, UPD_EXPRESSION_START_SKIP_SET, UPD_SKIP_FLAG_FO);
	// Skip all switch character (if found)
	const wchar_t* switch_skip = upd_skip(current, UPD_SWITCH_START_SKIP_SET, UPD_SKIP_FLAG_ALL);

	// token_type = EXPRESSION
	if (exp_skip != NULL){
		current = exp_skip;

		(const wchar_t*)token_value = upd_range(current, UPD_EXPRESSION_END_SKIP_SET, UPD_SKIP_FLAG_LO, &token_value_size);
		if (token_value == NULL) {
			ax_log_status(AX_INVALID_DATA, 1, NULL, L"Syntax Error: Expression not ended.");
			return AX_INVALID_DATA;
		}

		token_type = EXPRESSION;
	}
	// token_type = SWITCH 
	else if (switch_skip != NULL){
		current = switch_skip;

		(const wchar_t*)token_value = upd_range(current, UPD_SWITCH_END_SKIP_SET, UPD_SKIP_FLAG_LO | UPD_SKIP_FLAG_ALWAYS_RET, &token_value_size);
		if (token_value == NULL) {
			ax_log_status(AX_INVALID_DATA, 1, NULL, L"Syntax Error: Switch not ended.");
			return AX_INVALID_DATA;
		}

		token_type = SWITCH;
	}
	// token_type = VALUE 
	else {
		token_type = VALUE;
	}

	*token = malloc(sizeof(UPD_COMMAND_TOKEN));
	(*token)->value = token_value;
	(*token)->value_size = token_value_size;
	(*token)->token_type = token_type;

	return AX_SUCCESS;
}

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		expression,
	AX_OUT size_t*			buffer_size,
	AX_OUT void**			buffer
){
	return AX_SUCCESS;
}
AXSTATUS upd_execute_switch(
	AX_IN const wchar_t* 		string,
	AX_OUT size_t*			buffer_size,
	AX_OUT void**			buffer
){
	if (string == NULL ||
		buffer_size == NULL ||
		buffer == NULL){
		return AX_INVALID_ARGUMENT;
	}

	UPD_SWITCH switch_value = 0;
	
	if (wcscmp(string, UPD_SWITCH_INSTALL_STR) == 0){
		switch_value = UPD_SWITCH_INSTALL;	
	}
	else if (wcscmp(string, UPD_SWITCH_UPDATE_STR) == 0){
		switch_value = UPD_SWITCH_UPDATE;	
	}
	else{
		return AX_INVALID_DATA;
	}

	*buffer_size = sizeof(UPD_SWITCH);
	*buffer = malloc(*buffer_size);
	*(UPD_SWITCH*)(*buffer) = switch_value;

	return AX_SUCCESS;
}

const wchar_t* upd_range(
	AX_IN const wchar_t*		from,
	AX_IN const wchar_t 		skip_set[], // Array of characters that stop range lookup
	AX_IN uint16_t 			skip_flag,
	AX_OUT size_t*			range_size 			
){
	if (from == NULL ||
		skip_set == NULL ||
		range_size == NULL){
		return NULL;
	}

	const wchar_t* current = from;
	const wchar_t* start = from;
	const wchar_t* occurence;

	current = upd_skip(start, skip_set, skip_flag);

	wchar_t* range_buffer = NULL;
	size_t range_buffer_size = 0;
	if (current != NULL){
		range_buffer_size = current - start;
		range_buffer = malloc(range_buffer_size * sizeof(wchar_t));
		memcpy(range_buffer, start, range_buffer_size * sizeof(wchar_t));
		range_buffer[range_buffer_size] = '\0';

		*range_size = range_buffer_size;
	}

	return current == NULL ? NULL : range_buffer;
}
const wchar_t* upd_skip(
	AX_IN const wchar_t*		string,
	AX_IN const wchar_t 		skip_set[], // Array of characters to skip
	AX_IN uint16_t 			skip_flag
){
	if (string == NULL ||
		skip_set == NULL){
		return NULL;
	}

	const wchar_t* current = string;
	const wchar_t* occurence = NULL;

	uint8_t null_out = 0;

	switch (UPD_SKIP_FLAG_LOWER(skip_flag)){
	case UPD_SKIP_FLAG_FO:{
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			current++;
			if (occurence != NULL) break; 
		}

		if (occurence == NULL) null_out = 1;
		break;
	}
	case UPD_SKIP_FLAG_LO:{
		const wchar_t* last_occurence = NULL;
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			if (occurence != NULL) last_occurence = current; 
			current++;
		}

		if (last_occurence == NULL) null_out = 1;
		else current = last_occurence;
		break;
	}
	case UPD_SKIP_FLAG_ALL:{
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			if (occurence == NULL) break; 
			current++;
		}

		break;
	}
	default:
		break;
	}

	uint16_t skip_flag_bits = UPD_SKIP_FLAG_UPPER(skip_flag);
	if ((skip_flag_bits & UPD_SKIP_FLAG_ALWAYS_RET) != 0){
		return current == NULL ? NULL : current;
	}

	return null_out ? NULL : current;
}

