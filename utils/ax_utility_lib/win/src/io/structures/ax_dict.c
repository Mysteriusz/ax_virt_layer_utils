#include "ax_dict.h"
#include "murmur.h"

axres ax_dict_init(
	_in u32				exp_max, // Maximum expected count of values
	_out ax_dict			**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	ax_dict *dict = axmalloc(sizeof(ax_dict));
	dict->type = structure_seq;
	dict->count = 0;
	dict->chain_count = (exp_max * 4 + 3 - 1) / 3; // Around ~0.75 factor (exp_max = 100; chain_count = 124)
	dict->root = axmalloc(dict->chain_count * sizeof(ax_dict_chain));

	dict->add = (ax_structures_add_kv)ax_dict_add;
	dict->contains_key = (ax_structures_contains_k)ax_dict_contains_key;
	dict->remove = (ax_structures_remove_kv)ax_dict_remove;
	dict->at = (ax_structures_at_k)ax_dict_at;
	dict->query_at = (ax_structures_query_at_k)ax_dict_query_at;
	dict->clear = (ax_structures_clear)ax_dict_clear;
	dict->delete = (ax_structures_delete)ax_dict_delete;

	*buf = dict;

	return AX_SUCC;
}

axres ax_dict_add(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size,
	_in void 			*value,
	_in u64 			value_size
){
	if (dict == nullptr
	|| key == nullptr
	|| value == nullptr){
		return AX_INV_ARG;
	}

	// Compute chain index
	u64 hash = murmur_x64_128(key, key_size, 0);
	u32 i = hash % dict->chain_count;

	ax_dict_chain *chain = &dict->root[i];

	/*
		Find correct node in the chain
	*/
	ax_dict_node *prev = nullptr;
	ax_dict_node *node = chain->head;
	while(node != nullptr){
		// Break when the dictionary already contains the key (Value is overwritten)
		if(_sfmemcmp(node->key, key, node->key_size, key_size) == 0){
			break;
		}

		prev = node;
		node = node->next;
	}

	node = axmalloc(sizeof(ax_dict_node));

	/*
		Copy to internal buffer
	*/
	node->key = axmalloc(key_size);
	memcpy(node->key, key, key_size);
	node->key_size = key_size;

	node->value = axmalloc(value_size);
	memcpy(node->value, value, value_size);
	node->value_size = value_size;

	// Chain node insertion
	if (prev == nullptr){
		chain->head = node;
	}else{
		prev->next = node;
	}
	dict->count++;

	return AX_SUCC;
}

axres ax_dict_remove(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
){
	if (dict == nullptr
	|| key == nullptr){
		return AX_INV_ARG;
	}

	// Compute chain index
	u64 hash = murmur_x64_128(key, key_size, 0);
	u32 i = hash % dict->chain_count;

	ax_dict_chain *chain = &dict->root[i];

	/*
		Find correct node in the chain
	*/
	ax_dict_node *prev = nullptr;
	ax_dict_node *node = chain->head;
	while(node != nullptr){
		if(_sfmemcmp(node->key, key, node->key_size, key_size) == 0){
			break;
		}

		prev = node;
		node = node->next;
	}
	// No key occurence in the dictionary
	if (node == nullptr){
		return AX_NOT_FND;
	}

	// Unlink
	if(prev == nullptr){
		chain->head = nullptr;
	}else{
		prev->next = node->next;
	}

	dict->count--;

	/*
	 	Allocated cleanup
	*/
	axfree(node->value);
	axfree(node->key);
	axfree(node);
	
	return AX_SUCC;
}

axres ax_dict_contains_key(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
){
	if (dict == nullptr
	|| key == nullptr){
		return AX_INV_ARG;
	}

	// Compute chain index
	u64 hash = murmur_x64_128(key, key_size, 0);
	u32 i = hash % dict->chain_count;

	/*
		Find correct node in the chain
	*/
	ax_dict_chain *chain = &dict->root[i];
	ax_dict_node *node = chain->head;
	while(node != nullptr){
		if (_sfmemcmp(node->key, key, node->key_size, key_size) == 0){
			break;
		}

		node = node->next;
	}

	return (node != nullptr)
		? AX_SUCC
		: AX_NOT_FND;
}

axres ax_dict_at(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size,
	_out const ax_dict_node		**buf
){
	// Compute chain index
	u64 hash = murmur_x64_128(key, key_size, 0);
	u32 i = hash % dict->chain_count;

	/*
		Find correct node in the chain
	*/
	ax_dict_chain *chain = &dict->root[i];
	ax_dict_node *node = chain->head;
	while(node != nullptr){
		if (_sfmemcmp(node->key, key, node->key_size, key_size) == 0){
			break;
		}

		node = node->next;
	}

	if (node == nullptr){
		return AX_NOT_FND;
	}
	*buf = node;

	return AX_SUCC;
}
const void *ax_dict_query_at(
	_in ax_dict 			*dict,
	_in void 			*key,
	_in u64 			key_size
){
	if(dict == nullptr
	|| key == nullptr){
		return nullptr;
	}

	// Compute chain index
	u64 hash = murmur_x64_128(key, key_size, 0);
	u32 i = hash % dict->chain_count;

	ax_dict_chain *chain = &dict->root[i];
	ax_dict_node *node = chain->head;

	while(node != nullptr){
		if (_sfmemcmp(node->key, key, node->key_size, key_size) == 0){
			break;
		}

		node = node->next;
	}

	return (node != nullptr)
		? node->value
		: nullptr;
}

axres ax_dict_clear(
	_in ax_dict 			*dict
){
	if(dict == nullptr){
		return AX_INV_ARG;
	}

	/*
		Iterate over all chains
	*/
	u32 i = 0;
	ax_dict_chain *chain = dict->root; 
	ax_dict_node *node = nullptr; 
	ax_dict_node *next = nullptr; 
	while (i++ < dict->chain_count){
		node = chain->head;

		/*
		 	Iterate over chain nodes
		*/
		while(node != nullptr){
			next = node->next;
			axfree(node->key);
			axfree(node->value);
			axfree(node);
			node = next;
		}

		chain->head = nullptr;
		chain++;
	}
	dict->count = 0;

	return AX_SUCC;
}

axres ax_dict_delete(
	_in ax_dict 			*dict
){
	if (dict == nullptr){
		return AX_INV_ARG;
	}

	// Free all nodes
	axres res = AX_SUCC;
	res = ax_dict_clear(dict);
	if (AX_ERR(res)){
		return res;
	}
	axfree(dict->root);
	axfree(dict);

	return AX_SUCC;
}

