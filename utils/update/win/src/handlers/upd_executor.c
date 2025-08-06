#include "upd_executor.h"

AXSTATUS upd_execute_command(
	AX_IN const UPD_COMMAND*	command,
	AX_IN uint32_t			start_index,
	AX_OUT_OPT AXSTATUS* 		failed_token_status,
	AX_OUT_OPT uint32_t* 		failed_token_index
){
	if (command == NULL
		|| command->tokens == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS command_validation = upd_command_validate(command);
	if (AX_ERROR(command_validation)){
		ax_log_status(AX_INVALID_DATA, 1, NULL, L"COMMAND VALIDATION FAILED");
		return AX_INVALID_DATA;
	}

	uint64_t switch_exclusion_flags = 0;
	void* stack = NULL;
	size_t stack_size = 0;

	AXSTATUS status = AX_SUCCESS;

	uint32_t i = start_index;
	uint32_t j = i + 1;
	while (i < command->token_count){
		UPD_COMMAND_TOKEN* current = command->tokens[i];
		AXSTATUS current_result = AX_SUCCESS;

		switch (current->token_type){
		case VALUE:{
			ax_log_status(AX_INVALID_DATA, 0, NULL, L"INVALID SYMBOL");
			return AX_INVALID_DATA;
		}
		case SWITCH:{
			// Try to collect the stack
			stack = upd_collect_stack(command, &stack_size, &j);	
			status = upd_execute_switch(
				current->value,
				&switch_exclusion_flags,
				stack,
				stack_size,
				&current_result
			);

			// Stop command execution if there was an error
			if (AX_ERROR(status)
				|| AX_ERROR(current_result)){

				if (failed_token_status) *failed_token_status = current_result; 
				if (failed_token_index) *failed_token_index = i; 

				ax_log_status(current_result, 0, NULL, NULL);
				ax_log_status(status, 0, NULL, NULL);

				return status; 
			}

			break;
		}
		case EXPRESSION:{
			return AX_NOT_IMPLEMENTED;
		}
		default:
			return AX_INVALID_DATA;
		}

		i = j;
		j = i + 1;
	}
	
	return AX_SUCCESS;
}

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		string,
	AX_IN_OUT uint64_t*		switch_exclusion_flags,
	AX_IN_OPT void*			action_stack,
	AX_IN_OPT size_t		action_stack_size,
	AX_OUT_OPT AXSTATUS*		action_result
){
	if (string == NULL
		|| switch_exclusion_flags == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const UPD_SWITCH_DESCRIPTOR* current = upd_map_switch(string);
	if (current == NULL){
		return AX_NOT_FOUND;
	}

	UPD_COMMAND_TOKEN token = {
		.value = (void*)string,
		.value_size = wcslen(string),
		.token_type = SWITCH
	};
	AXSTATUS validation = upd_token_validate(&token, switch_exclusion_flags, action_stack, action_stack_size);
	if (AX_ERROR(validation)){
		return AX_NOT_IMPLEMENTED;
	}

	// Execute the action with provided stack (if needed)
	AXSTATUS result = current->action(action_stack);
	if (action_result) *action_result = result;

	if (AX_ERROR(result)){
		return AX_EXECUTION_ERROR;
	}

	// Add the executed switch exclusion flags
	*switch_exclusion_flags |= current->exclusion_flags;

	return AX_SUCCESS;
}
AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		string,
	AX_OUT AXSTATUS*		result
){
	if (string == NULL 
		|| result == NULL){
		return AX_INVALID_ARGUMENT;
	}

	return AX_SUCCESS;
}

