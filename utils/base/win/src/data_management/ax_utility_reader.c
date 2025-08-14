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

	AXSKIPSET* skip_set_buffer = 
		AX_SAFE_GET_SKIP_SET(settings_buffer); 

	status = ax_skip_set(
		&text,
		*skip_set_buffer
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
		*skip_set_buffer
	);
	if (AX_ERROR(status)){
		return status;
	}

	wchar_t* value_buffer = NULL;
	size_t value_buffer_span = 0;

	status = ax_skip_span(
		text,
		*skip_set_buffer,
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
	AX_IN AXSKIPSET				skip_set, 
	AX_OUT size_t*				span
){
	if (text == NULL
	|| skip_set == NULL
	|| span == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* original = text;

	uint32_t text_index = 0;

	size_t text_size = _ax_size_w(original);

	bool found = false;
	while (text_index < text_size
		&& text[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(skip_set); i++){
			found = (skip_set[i] == text[text_index])
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
	AX_IN AXSKIPSET				skip_set 
){
	if (text == NULL
	|| *text == NULL
	|| skip_set == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* original = *text;

	uint32_t text_index = 0;

	size_t text_size = _ax_size_w(original);

	bool found = false;
	while (text_index < text_size
		&& (*text)[text_index] != L'\0'){
		for (uint32_t i = 0; i < wcslen(skip_set); i++){
			found = (skip_set[i] == (*text)[text_index])
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

