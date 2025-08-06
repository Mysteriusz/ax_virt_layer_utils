#include "upd_parser.h"

AXSTATUS upd_command_parse(
	AX_IN const wchar_t** 	 	args,
	AX_IN uint32_t			arg_count,
	AX_OUT UPD_COMMAND**		command
){
	if (args == NULL 
		|| command == NULL){
		return AX_INVALID_ARGUMENT;
	}

	UPD_COMMAND* command_buffer = malloc(sizeof(UPD_COMMAND));
	command_buffer->tokens = malloc(sizeof(UPD_COMMAND_TOKEN*) * arg_count);
	command_buffer->token_count = arg_count;

	for (uint32_t i = 0; i < arg_count; i++){
		UPD_COMMAND_TOKEN* token = NULL;
		AXSTATUS token_status = upd_token_parse(args[i], &token);

		if (AX_ERROR(token_status)){
			upd_command_free(command_buffer);
			return AX_EXECUTION_ERROR;
		}

		command_buffer->tokens[i] = token;
	}

	*command = command_buffer;

	return AX_SUCCESS;
}

AXSTATUS upd_token_parse(
	AX_IN const wchar_t*	 	value,
	AX_OUT UPD_COMMAND_TOKEN**	token
){
	if (value == NULL 
		|| token == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* current = value;
	const wchar_t* skip = NULL;

	// Skip all leading white spaces
	skip = upd_skip(
		current, 
		UPD_EMPTY_SKIP_SET, 
		UPD_SKIP_FLAG_ALL
	);

	if (skip != NULL) current = skip;
		
	void* token_value = NULL;
	size_t token_value_size = 0;
	UPD_COMMAND_TOKEN_TYPE token_type = SWITCH; 

	// Skip all leading token entry characters
	size_t token_size = 0;
	const wchar_t* token_start = upd_skip(
		current, 
		UPD_TOKEN_START_SKIP_SET, 
		UPD_SKIP_FLAG_FO
	);

	// Token is starting with its start characters meaning there has to be an end
	if (token_start != NULL){
		current = upd_range(
			token_start, 
			UPD_TOKEN_END_SKIP_SET, 
			UPD_SKIP_FLAG_LO, 
			&token_size
		); 
		if (current == NULL){
			ax_log_status(
				AX_INVALID_DATA, 
				1, 
				NULL, 
				L"Syntax Error: Token not ended."
			);
			return AX_INVALID_DATA;
		}
	}
	//printf("%ls\n", current);

	// Skip first expression character (if found)
	const wchar_t* exp_skip = upd_skip(
		current, 
		UPD_EXPRESSION_START_SKIP_SET, 
		UPD_SKIP_FLAG_FO
	);
	// Skip all switch character (if found)
	const wchar_t* switch_skip = upd_skip(
		current, 
		UPD_SWITCH_START_SKIP_SET, 
		UPD_SKIP_FLAG_ALL
	);

	// token_type = EXPRESSION
	if (exp_skip != NULL){
		current = exp_skip;

		token_value = upd_range(
			current, 
			UPD_EXPRESSION_END_SKIP_SET, 
			UPD_SKIP_FLAG_LO, 
			&token_value_size
		);
		if (token_value == NULL) {
			ax_log_status(
				AX_INVALID_DATA, 
				1, 
				NULL, 
				L"Syntax Error: Expression not ended."
			);
			return AX_INVALID_DATA;
		}

		token_type = EXPRESSION;
	}
	// token_type = SWITCH 
	else if (switch_skip != NULL){
		//printf("%ls\n", L"switch");
		current = switch_skip;

		token_value = upd_range(
			current,
			UPD_SWITCH_END_SKIP_SET, 
			UPD_SKIP_FLAG_LO | UPD_SKIP_FLAG_ALWAYS_RET, 
			&token_value_size
		);
		if (token_value == NULL) {
			ax_log_status(
				AX_INVALID_DATA, 
				1, 
				NULL, 
				L"Syntax Error: Switch not ended."
			);
			return AX_INVALID_DATA;
		}

		token_type = SWITCH;
	}
	// token_type = VALUE 
	else {
		//printf("%ls\n", L"value");
		token_value = upd_range(
			current,
			(const wchar_t[]){'\0'}, 
			UPD_SKIP_FLAG_FO | UPD_SKIP_FLAG_ALWAYS_RET, 
			&token_value_size
		);
		token_type = VALUE;
	}

	*token = malloc(sizeof(UPD_COMMAND_TOKEN));
	(*token)->value = token_value;
	(*token)->value_size = token_value_size;
	(*token)->token_type = token_type;

	return AX_SUCCESS;
}

void upd_command_free(
	AX_IN_OUT UPD_COMMAND*		command
){
	if (command == NULL) return;

	for (uint32_t i = 0; i < command->token_count; i++){
		upd_token_free(command->tokens[i]);	
	}
	memset(command, 0, sizeof(UPD_COMMAND));
	free(command);
}
void upd_token_free(
	AX_IN_OUT UPD_COMMAND_TOKEN*	token
){
	if (token == NULL) return;

	free(token->value);
	memset(token, 0, sizeof(UPD_COMMAND_TOKEN));
	free(token);
}

AXSTATUS upd_command_validate(
	AX_IN const UPD_COMMAND*	command,
	AX_IN uint32_t			start_index
){
	if (command == NULL){
		return AX_INVALID_ARGUMENT;
	}

	uint32_t i = start_index;
	uint32_t j = i + 1;
	uint64_t exclusion_flags = 0;
	while (i < command->token_count){
		UPD_COMMAND_TOKEN* current = command->tokens[i];
		size_t current_stack_size = 0;
		void* current_stack = upd_collect_stack(command, &current_stack_size, &j);

		AXSTATUS current_status = upd_token_validate(current, &exclusion_flags, current_stack, current_stack_size);
		if (AX_ERROR(current_status)){
			return current_status;
		}

		i = j;
		j = i + 1;
	}

	return AX_SUCCESS;
}
AXSTATUS upd_token_validate(
	AX_IN const UPD_COMMAND_TOKEN*	token,
	AX_IN_OUT uint64_t*		exclusion_flags,
	AX_IN_OPT void* 		data_stack,
	AX_IN_OPT size_t		data_stack_size
){
	if (token == NULL
		|| exclusion_flags == NULL){
		return AX_INVALID_ARGUMENT;
	}

	switch (token->token_type){
	case SWITCH:{
		const UPD_SWITCH_DESCRIPTOR* descriptor = upd_map_switch((const wchar_t*)token->value);
		if (descriptor == NULL){
			return AX_NOT_FOUND;
		}

		// If exclusion flags contains the switch flags
		if (descriptor->switch_flags & *exclusion_flags){
			return AX_INVALID_DATA;
		}

		// If stack is required and provided stack is NULL
		if (descriptor->switch_flags & UPD_SWITCH_ACTION_STACK
			&& data_stack == NULL){
			return AX_INVALID_STACK;
		}

		// If required stack size is not equal to provided stack size
		if (descriptor->stack_size != data_stack_size){
			return AX_INVALID_STACK_SIZE;
		}

		*exclusion_flags |= descriptor->exclusion_flags; 
		break;
	}
	default:
		return AX_INVALID_ARGUMENT;
	}
	
	return AX_SUCCESS;
}

wchar_t* upd_range(
	AX_IN const wchar_t*		from,
	AX_IN const wchar_t 		skip_set[], // Array of characters that stop range lookup
	AX_IN uint16_t 			skip_flag,
	AX_OUT size_t*			range_size 			
){
	if (from == NULL 
		|| skip_set == NULL 
		|| range_size == NULL){
		return NULL;
	}

	const wchar_t* current = from;
	const wchar_t* start = from;

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
	if (string == NULL 
		|| skip_set == NULL){
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
		uint32_t skipped = 0;
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			if (occurence == NULL) break; 
			skipped++;
			current++;
		}

		if (skipped == 0) null_out = 1;
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

