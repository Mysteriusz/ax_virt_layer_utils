#include "ax_utility_reader.h"

AXSTATUS ax_mode_offset(
	AX_IN uint32_t				start_index,
	AX_IN AXREADMODE			mode,
	AX_OUT int32_t* 			index_offset
){
	int32_t index_buffer = 0;
	switch (mode){
	case AX_READ_BEFORE_FIRST:
		index_buffer = (start_index != 0)
			? -1
			: 0; 
		break;
	case AX_READ_BEFORE_LAST:
		index_buffer = (start_index != 0)
			? -1
			: 0; 
		break;
	case AX_READ_AFTER_FIRST:
		index_buffer = 1;
		break;
	case AX_READ_AFTER_LAST:
		index_buffer = 1;
		break;
	case AX_READ_AT_FIRST:
		index_buffer = 0;
		break;
	case AX_READ_AT_LAST:
		index_buffer = 0;
		break;
	default:
		return AX_INVALID_DATA;
	}

	*index_offset = index_buffer;
	return AX_SUCCESS;
}
AXSTATUS ax_compare_range(
	AX_IN const wchar_t*			text,
	AX_IN const wchar_t*			word
){
	if (text == NULL
	|| word == NULL){
		return AX_INVALID_ARGUMENT;
	}

	bool found = false;

	const wchar_t* text_char = text;
	const wchar_t* word_char = word;

	while(*text_char == *word_char
	&& *text_char != L'\0'){
		word_char++;
		text_char++;

		if (*word_char == L'\0'){
			found = true;	
			break;
		}
	}

	return (found == true) 
		? AX_SUCCESS 
		: AX_NOT_FOUND;
}

AXSTATUS ax_in_charset(
	AX_IN const wchar_t			value,
	AX_IN AXCHARSET				char_set
){
	if (char_set == NULL){
		return AX_INVALID_ARGUMENT;
	}

	bool found = false;
	wchar_t* current = char_set;

	while(*current != L'\0'){
		if (*current == value){
			found = true;	
			break;
		}
		current++;
	}

	return (found == true) 
		? AX_SUCCESS 
		: AX_NOT_FOUND;
}
