#include "upd_executor.h"

void* upd_collect_stack(
	AX_IN const UPD_COMMAND*	command,
	AX_OUT size_t*			stack_size,
	AX_IN_OUT uint32_t* 		token_index
){
	if (command == NULL
		|| command->tokens == NULL
		|| stack_size == NULL
		|| token_index == NULL){
		return NULL;
	}

	void* stack_buffer = NULL;
	size_t stack_buffer_size = 0;
	uint32_t i = *token_index;

	while (i < command->token_count){
		UPD_COMMAND_TOKEN* current = command->tokens[i];
		if (current->token_type != VALUE) break;

		stack_buffer_size += current->value_size;
		void* temp = realloc(stack_buffer, stack_buffer_size);
		stack_buffer = temp;
	 
		i++;
	}

	*stack_size = stack_buffer_size;
	*token_index = i;

	return stack_buffer;
}

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

	uint64_t switch_exclusion_flags = 0;
	void* stack = NULL;
	size_t stack_size = 0;

	AXSTATUS status = AX_SUCCESS;

	uint32_t i = start_index;
	while (i < command->token_count){
		UPD_COMMAND_TOKEN* current = command->tokens[i];
		AXSTATUS current_result = AX_SUCCESS;
		uint32_t j = i + 1;

		switch (current->token_type){
		case VALUE:{
			ax_log_status(AX_INVALID_DATA, 0, NULL, L"INVALID SYMBOL");
			return AX_INVALID_DATA;
		}
		case SWITCH:{
			status = upd_execute_switch(
				current->value,
				&switch_exclusion_flags,
				NULL,
				0,
				&current_result
			);
			// That means the switch requires stack 
			if (status == AX_INVALID_STACK){
				// Collect all the next tokens with name VALUE 
				stack = upd_collect_stack(command, &stack_size, &j);	
				status = upd_execute_switch(
					current->value,
					&switch_exclusion_flags,
					stack,
					stack_size,
					&current_result
				);
			}

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
	}
	
	return AX_SUCCESS;
}

const UPD_SWITCH_DESCRIPTOR* upd_map_switch(
	AX_IN const wchar_t*		string
){
	const UPD_SWITCH_DESCRIPTOR* switch_buffer = NULL;

	for (uint32_t i = 0; i < UPD_SWITCH_TABLE_COUNT; i++){
		UPD_SWITCH_DESCRIPTOR current = UPD_SWITCH_TABLE[i];
		if (wcscmp(current.switch_string, string) == 0){
			switch_buffer = &current;
			break;
		}
	}

	return switch_buffer;
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

	// If exclusion flags contains the switch flags
	if (current->switch_flags & *switch_exclusion_flags){
		return AX_INVALID_DATA;
	}

	// If stack is required and provided stack is NULL
	if (current->switch_flags & UPD_SWITCH_ACTION_STACK
		&& action_stack == NULL){
		return AX_INVALID_STACK;
	}
	// If required stack size is not equal to provided stack size
	if (current->stack_size != action_stack_size){
		return AX_INVALID_STACK_SIZE;
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

