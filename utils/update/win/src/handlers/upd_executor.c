#include "upd_executor.h"

AXSTATUS upd_execute_switch(
	AX_IN const wchar_t*		switch_string,
	AX_OUT AXSTATUS*		switch_result
){
	if (switch_string == NULL 
		|| switch_result == NULL){
		return AX_INVALID_ARGUMENT;
	}

	return AX_SUCCESS;
}
AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		expression_string,
	AX_OUT AXSTATUS*		expression_result
){
	if (expression_string == NULL 
		|| expression_result == NULL){
		return AX_INVALID_ARGUMENT;
	}

	return AX_SUCCESS;
}

