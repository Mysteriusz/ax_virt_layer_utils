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
	const wchar_t* skip = NULL;

	skip = upd_skip(current, UPD_EMPTY_SKIP_SET, UPD_SKIP_FLAG_ALL);
	if (skip != NULL) current = skip;
		
	void* token_value = NULL;
	size_t token_value_size = 0;
	UPD_COMMAND_TOKEN_TYPE token_type = SWITCH; 

	while(*current != '\0'){
		skip = upd_skip(current, UPD_TOKEN_START_SKIP_SET, UPD_SKIP_FLAG_ALL);
		if (skip != NULL) current = skip;

		skip = upd_skip(current, UPD_EXPRESSION_START_SKIP_SET, UPD_SKIP_FLAG_FO);
		if (skip != NULL){
			current = skip;
		
			const wchar_t* expression_start = current; 

			skip = upd_skip(current, UPD_EXPRESSION_END_SKIP_SET, UPD_SKIP_FLAG_LO);
			if (skip != NULL){
				current = skip;
			}
			else{
				printf("%ls", L"Syntax error: Expression not ended");
			}

			const wchar_t* expression_end = current; 

			size_t expression_size = expression_end - expression_start;
			wchar_t* expression = malloc(expression_size);
			memcpy(expression, expression_start, expression_size * sizeof(wchar_t));

			free(expression);

			printf("%llu\n", expression_size);
			printf("%ls: %ls\n", L"EXP", expression);
		}

		current++;
	}

	token = malloc(sizeof(UPD_COMMAND_TOKEN));
	token->value = token_value;
	token->value_size = token_value_size;
	token->token_type = token_type;

	return AX_SUCCESS;
}

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		expression,
	AX_OUT size_t*			buffer_size,
	AX_OUT void**			buffer
){
	return AX_SUCCESS;
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

	switch (skip_flag){
	case UPD_SKIP_FLAG_FO:{
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			current++;
			if (occurence != NULL) break; 
		}
		break;
	}
	case UPD_SKIP_FLAG_LO:{
		const wchar_t* last_occurence = NULL;
		while(*current != L'\0'){
			occurence = wcschr(skip_set, *current);
			if (occurence != NULL) last_occurence = current; 
			current++;
		}
		current = last_occurence;
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


	return current;
}

