#include "upd_executor.h"

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		string,
	AX_IN_OPT void*			stack,
	AX_IN_OPT size_t		stack_size,
	AX_OUT AXSTATUS*		result
){
	if (string == NULL 
		|| result == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS action_result = AX_SUCCESS;
	uint8_t switch_found = 0;

	for (uint32_t i = 0; i < sizeof(UPD_SWITCH_TABLE) / sizeof(UPD_SWITCH_DESCRIPTOR); i++){
		UPD_SWITCH_DESCRIPTOR current = UPD_SWITCH_TABLE[i];
		if (wcscmp(current.switch_string, string) == 0){
			if (current.action_stack_size != stack_size){
				return AX_INVALID_BUFFER_SIZE;
			}
			if (current.switch_flags & UPD_SWITCH_ACTION_STACK
				&& stack == NULL){
				return AX_INVALID_ARGUMENT;
			}

			action_result = current.action(stack);
			switch_found = 1;
			break;
		}
	}

	if (!switch_found){
		return AX_NOT_FOUND;
	}

	*result = action_result;

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

