#include "upd_utility.h"

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
const UPD_SWITCH_DESCRIPTOR* upd_map_switch(
	AX_IN const wchar_t*		string
){
	const UPD_SWITCH_DESCRIPTOR* switch_buffer = NULL;

	for (uint32_t i = 0; i < UPD_SWITCH_TABLE_COUNT; i++){
		if (wcscmp(UPD_SWITCH_TABLE[i].switch_string, string) == 0){
			switch_buffer = &UPD_SWITCH_TABLE[i];
			break;
		}
	}

	return switch_buffer;
}
