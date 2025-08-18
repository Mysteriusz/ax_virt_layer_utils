#include "ax_utility_reader.h"

AXSTATUS ax_read_range(
	AX_IN const wchar_t*			text,
	AX_IN AXREADFLAG			flags,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t**			buffer,
	AX_OUT size_t*				buffer_size
){
	if (text == NULL
	|| buffer == NULL
	|| buffer_size == NULL){
		return AX_INVALID_ARGUMENT;
	}

	/*
	 	Setup all buffers.
	*/

	AXSTATUS status = AX_SUCCESS;

	struct AX_READER_SETTINGS* settings_buffer = 
		AX_SAFE_GET_SETTINGS(settings); 
	AXCHARSET* char_set_buffer =
		AX_SAFE_GET_CHAR_SET(settings_buffer, flags);
	uint32_t min_index = 
		AX_SAFE_GET_MIN_INDEX(settings_buffer, flags); 
	uint32_t max_index =
		AX_SAFE_GET_MAX_INDEX(text, settings_buffer, flags); 

	AXREADMODE mode = (AXREADMODE)flags;
	if (AX_READ_MODE_INVALID(mode)){
		return AX_INVALID_DATA;
	}

	if (max_index <= min_index){
		return AX_INVALID_INDEX;
	}

	uint32_t text_index = min_index;
	uint32_t occurrence_index = 0;

	const wchar_t* text_char = &text[min_index];	
	const wchar_t* occurrence_char = NULL;

	bool found = false;

	while (text_index >= min_index
	&& text_index < max_index){
		found = (ax_in_charset(*text_char, *char_set_buffer) == AX_SUCCESS)
			? true
			: false;

		if (found == true){
			occurrence_char = text_char; 
			occurrence_index = text_index;
		}

		if (_ax_check_bit(flags, AX_READ_IN_PLACE) == true){
			break;
		}

		if (AX_READ_MODE_FIRST(mode)
		&& found == true){
			break;
		}

		text_index++;	
		text_char++;

		if (AX_READ_MODE_LAST(mode)
		&& text_index == max_index){
			break;
		}
	}

	found = (occurrence_char != NULL)
		? true
		: false;
	
	if (_ax_check_bit(flags, AX_READ_RETURN) == false
	&& found == false){
		return AX_NOT_FOUND;
	}

	int32_t offset = 0;
	status = ax_mode_offset(
		occurrence_index,
		mode,
		&offset
	);
	if (AX_ERROR(status)){
		return status;
	}

	size_t range_buffer_size = 
		(found == true)
		? (_ax_size_w(&text[min_index]) - 1) - (occurrence_index + offset)
		: _ax_size_w(&text[min_index]);

	wchar_t* range_buffer = malloc((range_buffer_size + 1) * sizeof(wchar_t));
	memcpy(range_buffer, occurrence_char + offset, range_buffer_size * sizeof(wchar_t));

	range_buffer[range_buffer_size] = L'\0';

	*buffer = range_buffer;
	*buffer_size = range_buffer_size;

	return (found == true)
		? AX_SUCCESS
		: AX_PARTIAL_ERROR;
}

AXSTATUS ax_skip_range(
	AX_IN const wchar_t*			text,
	AX_IN const wchar_t*			skip,
	AX_IN AXREADFLAG			flags,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT const wchar_t**			buffer,
	AX_OUT_OPT uint32_t*			skipped	
){
	if (text == NULL
	|| buffer == NULL){
		return AX_INVALID_ARGUMENT;
	}

	AXSTATUS status = AX_SUCCESS;

	/*
	 	Setup all buffers.
	*/

	struct AX_READER_SETTINGS* settings_buffer = 
		AX_SAFE_GET_SETTINGS(settings); 
	AXCHARSET* char_set_buffer =
		AX_SAFE_GET_CHAR_SET(settings_buffer, flags);
	uint32_t min_index = 
		AX_SAFE_GET_MIN_INDEX(settings_buffer, flags); 
	uint32_t max_index =
		AX_SAFE_GET_MAX_INDEX(text, settings_buffer, flags); 

	AXREADMODE mode = (AXREADMODE)flags;
	if (AX_READ_MODE_INVALID(mode)){
		return AX_INVALID_DATA;
	}

	if (max_index <= min_index){
		return AX_INVALID_INDEX;
	}

	uint32_t text_index = min_index; 
	uint32_t occurrence_index = 0; 

	const wchar_t* text_char = &text[min_index];
	const wchar_t* occurrence_char = NULL;

	bool found = false;

	while (text_index >= min_index
	&& text_index < max_index){
		// Check if current character extends to the entire skip buffer.	
		found = (ax_compare_range(text_char, skip) == AX_SUCCESS)
			? true
			: false;

		if (found == true){
			occurrence_char = text_char;
			occurrence_index = text_index;
		}

		if (_ax_check_bit(flags, AX_READ_IN_PLACE) == true){
			break;
		}

		if (AX_READ_MODE_FIRST(mode)
		&& found == true){
			break;
		}

		text_index++;	
		text_char++;

		if (AX_READ_MODE_LAST(mode)
		&& text_index == max_index){
			break;
		}
	}

	found = (occurrence_char != NULL)
		? true
		: false;

	if (found == false
	&& _ax_check_bit(flags, AX_READ_RETURN) == false){
		return AX_NOT_FOUND;
	}

	int32_t offset = 0;
	status = ax_mode_offset(
		occurrence_index,
		mode,
		&offset
	);
	if (AX_ERROR(status)){
		return status;
	}

	if (AX_READ_MODE_AFTER(mode)){
		occurrence_char += wcslen(skip) - 1;
		occurrence_index += (uint32_t)(wcslen(skip) - 1);
	}

	const wchar_t* skipped_buffer = occurrence_char + offset;
	uint32_t skipped_index = occurrence_index + offset;

	uint32_t skipped_char_count = 0;

	status = ax_skip_charset(
		skipped_buffer,
		*char_set_buffer,
		&skipped_buffer,
		&skipped_char_count
	);
	if (AX_ERROR(status)){
		return status;
	}

	skipped_index += skipped_char_count;

	*buffer = 
		(found == true)
		? skipped_buffer 
		: text;

	if (skipped != NULL){
		*skipped = 
			(found == true)
			? skipped_index 
			: 0;
	}

	return (found == true)
		? AX_SUCCESS
		: AX_PARTIAL_ERROR;
}

AXSTATUS ax_skip_charset(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set,
	AX_OUT const wchar_t**			buffer,
	AX_OUT_OPT uint32_t*			skipped
){
	if (text == NULL
	|| char_set == NULL
	|| buffer == NULL){
		return AX_INVALID_ARGUMENT;
	}

	const wchar_t* text_char = text;

	while(ax_in_charset(*text_char, char_set) == AX_SUCCESS){
		text_char++;
	}

	*buffer = text_char;
	if (skipped != NULL){
		*skipped = (uint32_t)(text_char - text);
	}

	return AX_SUCCESS;
}

