#if !defined(UPD_EXECUTOR_INT)
#define UPD_EXECUTOR_INT

#include "upd_parser.h"

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		string,
	AX_IN_OPT void*			stack,
	AX_IN_OPT size_t		stack_size,
	AX_OUT AXSTATUS*		result
);

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		string,
	AX_OUT AXSTATUS*		result
);

#endif // !defined(UPD_EXECUTOR_INT)

