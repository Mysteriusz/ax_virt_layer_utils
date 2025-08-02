#include "upd_parser.h"

AXSTATUS upd_command_parse(
	AX_IN const wchar_t** 	 	args,
	AX_IN const uint32_t		arg_count,
	AX_OUT UPD_COMMAND*		command
){
	return AX_SUCCESS;
}

AXSTATUS upd_token_parse(
	AX_IN const wchar_t*	 	value,
	AX_IN const uint32_t		value_length,
	AX_OUT UPD_COMMAND_TOKEN*	token
){
	if (value == NULL || 
		token == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* current = value;
	const wchar_t* skip;

	skip = upd_skip(current, UPD_EMPTY_SKIP_SET);
	if (skip != NULL) current = skip;
		
	void* token_value = NULL;
	size_t token_value_size = 0;
	UPD_COMMAND_TOKEN_TYPE token_type = SWITCH; 

	while(*current != '\x0000'){
		skip = upd_skip(current, UPD_TOKEN_START_SKIP_SET);
		if (skip != NULL) current = skip;

		printf("%ls: %ls\n", L"TOKEN", current);
	
		skip = upd_skip(current, UPD_EXPRESSION_START_SKIP_SET);
		// Execute expression
		if (skip != NULL){
			current = skip;
			token_type = EXPRESSION;
			printf("%ls: %ls\n", L"EXPRESSION", current);
			
			skip = upd_skip(current, UPD_EXPRESSION_END_SKIP_SET);
			// Find expression ending
			if (skip == NULL){
				printf("%ls", L"Syntax error: Expression not ended");
				break;
			}

			current = skip;
		}
		
		current++;
		if (wcschr(UPD_TOKEN_END_SKIP_SET, *current)) break;
	}

	token = malloc(sizeof(UPD_COMMAND_TOKEN));
	token->value = token_value;
	token->value_size = token_value_size;
	token->token_type = token_type;

	return AX_SUCCESS;
}

const wchar_t* upd_skip(
	AX_IN const wchar_t*		string,
	AX_IN const wchar_t 		skip_set[] // Array of characters to skip
){
	if (string == NULL ||
		skip_set == NULL){
		return NULL;
	}

	const wchar_t* current = string;

	wchar_t* found = NULL;
	while (*current != L'\x0000' && found == NULL){
		found = wcschr(skip_set, *current);
		current++;
	}

	return found == NULL ? NULL : current;
}

