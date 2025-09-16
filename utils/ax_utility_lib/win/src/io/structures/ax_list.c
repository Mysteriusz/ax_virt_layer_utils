#include "ax_structures.h"

axres ax_list_init(
	_out ax_list			**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	ax_list *list = axmalloc(sizeof(ax_list));
	list->root = nullptr;
	list->count = 0;

	list->add = (ax_structures_add)ax_list_add;
	list->remove = (ax_structures_remove)ax_list_remove;
	list->delete = (ax_structures_delete)ax_list_delete;
	list->at = (ax_structures_at)ax_list_at;

	*buf = list;

	return AX_SUCC;
}
axres ax_list_add(
	_in ax_list *list,
	_in void *value,
	_in u64 size
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
		if (sfmemcmp(curr->value, value, curr->size, size) == 0){
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
axres ax_list_delete(
	_in ax_list 			*list
){
	ax_list_node *prev = nullptr;
	ax_list_node *curr = list->root;
	// Free all nodes
	while(curr != nullptr){
		prev = curr;
		curr = curr->next;

		axfree(prev->value);
		axfree(prev);
	}
	axfree(list);

	return AX_SUCC;
}

