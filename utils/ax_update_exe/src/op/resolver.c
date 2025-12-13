#include "resolver.h"

iter_code op_exec_iter(
	ax_list_iter_stack 	stack
){
	struct op_err *err = (struct op_err*)stack.data;
	struct op_desc *desc = (struct op_desc*)stack.node->value; 

	// Execute stack command
	*err = desc->call(&desc->meta);

	return ITER_NONE;
}

axres op_exec(
	_in ax_list		*op_list,
	_out struct op_err 	*err
){
	if (op_list == nullptr){
		return AX_INV_ARG;
	}

	op_list->iter(
		op_list,
		(ax_iter_act)op_exec_iter,
		err,
		nullptr);

	return AX_SUCC;
}

