#if !defined(UPD_EXECUTOR_INT)
#define UPD_EXECUTOR_INT

#include "ax_utility.h"
#include "upd_parser.h"

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		switch_string,
	AX_OUT AXSTATUS*		switch_result
);

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		expression_string,
	AX_OUT AXSTATUS*		expression_result
);

#endif // !defined(UPD_EXECUTOR_INT)

