#include "ax_list.h"

axres ax_list_init(
	_out ax_list			**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	ax_list *list = axmalloc(sizeof(ax_list));
	list->type = structure_seq;
	list->root = nullptr;
	list->count = 0;

	list->add = (ax_structures_add)ax_list_add;
	list->contains = (ax_structures_contains)ax_list_contains;
	list->remove = (ax_structures_remove)ax_list_remove;
	list->at = (ax_structures_at)ax_list_at;
	list->query_at = (ax_structures_query_at)ax_list_query_at;
	list->iter = (ax_structures_iter)ax_list_iter;
	list->clear = (ax_structures_clear)ax_list_clear;
	list->delete = (ax_structures_delete)ax_list_delete;

	*buf = list;

	return AX_SUCC;
}
axres ax_list_add(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
){
	if (list == nullptr
	|| value == nullptr){
		return AX_INV_ARG;
	}

	ax_list_node *prev = nullptr;
	ax_list_node *curr = list->root;
	while(curr != nullptr){
		prev = curr;
		curr = curr->next;
	}

	curr = axmalloc(sizeof(ax_list_node));

	// Copy to internal buffer
	curr->value = axmalloc(size);
	memcpy(curr->value, value, size);
	curr->size = size;

	// Write to list
	if (prev == nullptr){
		list->root = curr;
	}else{
		prev->next = curr;
	}
	list->count++;

	return AX_SUCC;
}
axres ax_list_contains(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
){
	if (list == nullptr
	|| value == nullptr){
		return AX_INV_ARG;
	}

	ax_list_node *curr = list->root;
	while(curr != nullptr){
		// Found 
		if (_sfmemcmp(curr->value, value, curr->size, size) == 0){
			break;
		}

		curr = curr->next;
	}

	return (curr != nullptr) 
		? AX_SUCC
		: AX_NOT_FND;
}
axres ax_list_remove(
	_in ax_list 			*list,
	_in void 			*value,
	_in u64 			size
){
	if (list == nullptr
	|| value == nullptr){
		return AX_INV_ARG;
	}

	ax_list_node *prev = nullptr;
	ax_list_node *curr = list->root;
	while(curr != nullptr){
		// Found 
		if (_sfmemcmp(curr->value, value, curr->size, size) == 0){
			break;
		}

		prev = curr;
		curr = curr->next;
	}

	if (curr == nullptr){
		return AX_NOT_FND;
	}

	// Unlink
	if (prev == nullptr){
		list->root = curr->next;
	}else{
		prev->next = curr->next;
	}

	list->count--;
	axfree(curr->value);
	axfree(curr);

	return AX_SUCC;
}
axres ax_list_at(
	_in const ax_list 		*list,
	_in u32 			index,
	_out const ax_list_node 	**buf
){
	if (list == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	u32 i = 0;
	ax_list_node *curr = list->root;
	while (curr != nullptr){
		// Found
		if (index == i){
			break;
		}

		i++;
		curr = curr->next;
	}

	if (curr == nullptr){
		return AX_NOT_FND;
	}

	*buf = curr;

	return AX_SUCC;
}
void *ax_list_query_at(
	_in const ax_list 		*list,
	_in u32 			index
){
	if (list == nullptr){
		return nullptr;
	}

	const ax_list_node *node = nullptr;
	list->at(list, index, (const void**)&node);

	if (node == nullptr){
		return nullptr;
	}

	return node->value;
}
axres ax_list_iter(
	_in const ax_list 		*list,
	_in ax_iter_act			action,
	_in_opt void 			*data,
	_out_opt const ax_list_node	**buf
){
	if (list == nullptr
	|| action == nullptr){
		return AX_INV_ARG;
	}

	iter_code code = ITER_NONE;
	ax_list_iter_stack stack = {0};

	ax_list_node *node = list->root;
	while(node != nullptr){
		// Execute action with list stack
redo_code:
		stack = (ax_list_iter_stack){
			.list = list,
			.node = node,
			.data = data,
		};
		code = action(
			(u8*)&stack
		);

		switch(code){
		case ITER_NONE:
			break;
		case ITER_REDO:
			goto redo_code;
		case ITER_STOP:
			goto stop_code;
		default:
			return AX_INV_CODE;
		}

		node = node->next;
	}
stop_code:
	if (buf != nullptr){
		*buf = node;
	}

	return AX_SUCC;
}
axres ax_list_clear(
	_in ax_list 			*list
){
	if (list == nullptr){
		return AX_INV_ARG;
	}

	ax_list_node *node = list->root;
	ax_list_node *next = nullptr;
	while(node != nullptr){
		next = node->next;
		axfree(node->value);
		axfree(node);
		node = next;
	}
	list->root = nullptr;

	return AX_SUCC;
}
axres ax_list_delete(
	_in ax_list 			*list
){
	if (list == nullptr){
		return AX_INV_ARG;
	}

	// Free all nodes
	axres res = AX_SUCC;
	res = ax_list_clear(list);
	if (AX_ERR(res)){
		return res;
	}
	axfree(list);

	return AX_SUCC;
}

