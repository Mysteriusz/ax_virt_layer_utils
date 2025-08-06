#if !defined(UPD_EXECUTOR_INT)
#define UPD_EXECUTOR_INT

#include "upd_utility.h"

// TODO: COPY TO THE STACK
void* upd_collect_stack(
	AX_IN const UPD_COMMAND*	command,
	AX_OUT size_t*			stack_size,
	AX_IN_OUT uint32_t* 		token_index
);

AXSTATUS upd_execute_command(
	AX_IN const UPD_COMMAND*	command,
	AX_IN uint32_t			start_index,
	AX_OUT_OPT AXSTATUS* 		failed_token_status,
	AX_OUT_OPT uint32_t* 		failed_token_index
);

const UPD_SWITCH_DESCRIPTOR* upd_map_switch(
	AX_IN const wchar_t*		string
);

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		string,
	AX_IN_OUT uint64_t*		switch_exclusion_flags,
	AX_IN_OPT void*			action_stack,
	AX_IN_OPT size_t		action_stack_size,
	AX_OUT_OPT AXSTATUS*		action_result
);

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		string,
	AX_OUT AXSTATUS*		result
);

#endif // !defined(UPD_EXECUTOR_INT)

