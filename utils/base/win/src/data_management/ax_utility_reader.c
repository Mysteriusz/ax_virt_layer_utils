#include "ax_utility_reader.h"

AXSTATUS ax_read_range(
	AX_IN const wchar_t*			text,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t**			buffer,
	AX_OUT size_t*				buffer_size
){
	if (text == NULL
	|| buffer == NULL
	|| buffer_size == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	struct AX_READER_SETTINGS* settings_buffer = 
		AX_SAFE_GET_SETTINGS(settings); 

	AXCHARSET* char_set_buffer = 
		AX_SAFE_GET_CHAR_SET(settings_buffer); 

	status = ax_skip_set(
		&text,
		*char_set_buffer
	);
	if (AX_ERROR(status)){
		return status;
	}
	
	status = ax_skip_text(
		&text,
		settings_buffer->label
	);
	if (AX_ERROR(status)){
		return status;
	}

	status = ax_skip_set(
		&text,
		*char_set_buffer
	);
	if (AX_ERROR(status)){
		return status;
	}

	wchar_t* value_buffer = NULL;
	size_t value_buffer_span = 0;

	status = ax_skip_span(
		text,
		*char_set_buffer,
		&value_buffer_span
	);
	if (AX_ERROR(status)){
		return status;
	}

	value_buffer_span = (value_buffer_span + 1) * sizeof(wchar_t);

	value_buffer = malloc(value_buffer_span);
	memcpy(value_buffer, text, value_buffer_span);

	value_buffer[value_buffer_span] = L'\0';

	*buffer = value_buffer;
	*buffer_size = value_buffer_span;

	return AX_SUCCESS;
}

AXSTATUS ax_skip_text(
	AX_IN_OUT const wchar_t**		text,
	AX_IN wchar_t*				skip
){
	if (text == NULL
	|| *text == NULL
	|| skip == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* original = *text;

	uint32_t text_index = 0;
	uint32_t skip_index = 0;

	size_t text_size = _ax_size_w(original);
	size_t skip_size = _ax_size_w(skip);

	while(text_index < text_size
		&& skip_index < skip_size
		&& (*text)[text_index] == skip[skip_index]
		&& (*text)[text_index] != L'\0'){
		text_index++;
		skip_index++;
	}

	*text = &original[text_index];

	return AX_SUCCESS;
}
AXSTATUS ax_skip_span(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set, 
	AX_OUT size_t*				span
){
	if (text == NULL
	|| char_set == NULL
	|| span == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* original = text;

	uint32_t text_index = 0;

	size_t text_size = _ax_size_w(original);

	bool found = false;
	while (text_index < text_size
		&& text[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(char_set); i++){
			found = (char_set[i] == text[text_index])
				? true
				: false;

			if (found == true){
				break;
			}
		}

		if (found == true){
			break;
		}

		text_index++;
	}

	*span = &original[text_index] - original;

	return AX_SUCCESS;
}
AXSTATUS ax_skip_set(
	AX_IN_OUT const wchar_t**		text,
	AX_IN AXCHARSET				char_set 
){
	if (text == NULL
	|| *text == NULL
	|| char_set == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* original = *text;

	uint32_t text_index = 0;
	size_t text_size = _ax_size_w(original);

	bool found = false;
	while (text_index < text_size
	&& (*text)[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(char_set); i++){
			found = (char_set[i] == (*text)[text_index])
				? true
				: false;

			if (found == true){
				break;
			}
		}

		if (found == false){
			break;
		}

		text_index++;
	}

	*text = &original[text_index];

	return AX_SUCCESS;
}

AXSTATUS ax_char_count(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set,
	AX_OUT size_t* 				count
){
	if (text == NULL
	|| char_set == NULL
	|| count == NULL){
		return AX_INVALID_ARGUMENT;
	}

	uint32_t text_index = 0;
	size_t text_size = _ax_size_w(text);

	size_t count_buffer = 0;	
	while (text_index < text_size
	&& text[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(char_set); i++){
			count_buffer = (char_set[i] == text[text_index])
				? count_buffer + 1
				: count_buffer;
		}

		text_index++;
	}

	*count = count_buffer;

	return AX_SUCCESS;
}

AXSTATUS ax_split_text(
	AX_IN const wchar_t*			text,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t***			buffer,
	AX_OUT size_t*				buffer_size
){
	if (text == NULL
	|| buffer == NULL
	|| buffer_size == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	struct AX_READER_SETTINGS* settings_buffer = AX_SAFE_GET_SETTINGS(settings);	
	AXCHARSET* char_set = AX_SAFE_GET_CHAR_SET(settings_buffer);	
	size_t text_size = _ax_size_w(text); 
	uint32_t text_index = 0; 

	bool split = false;
	uint32_t split_index = 0;	
	size_t split_size = 0;	
	wchar_t* split_buffer = 0;	

	size_t split_array_size = 0;
	size_t split_array_index = 0;
	wchar_t** split_array = NULL;

	status = ax_char_count(&text[text_index], *char_set, &split_array_size);
	if (AX_ERROR(status)){
		return status;
	}

	split_array_size++;
	split_array = malloc(split_array_size * sizeof(wchar_t*));

	if (split_array_size == 1){
		split_array[0] = (wchar_t*)text;
		return AX_PARTIAL_ERROR;
	}

	while (text_index < text_size
	&& split_array_index < split_array_size
	&& text[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(*char_set); i++){
			split = ((*char_set)[i] == text[text_index])
			? true
			: false;

			if (split == true){
				break;
			}
		}

		text_index++;

		if (split == true
		|| text[text_index] == L'\0'){
			split_size = text_index - split_index;
			if (text[text_index] != L'\0'){
				split_size--;
			}

			split_buffer = malloc((split_size + 1) * sizeof(wchar_t));
			memcpy(split_buffer, &text[split_index], split_size * sizeof(wchar_t));
			split_buffer[split_size] = L'\0';

			split_array[split_array_index] = split_buffer;

			split_array_index++;
			split_index = text_index;
		}
	}

	*buffer_size = split_array_size;
	*buffer = split_array;

	return AX_SUCCESS;
}

