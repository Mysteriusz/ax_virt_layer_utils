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

	struct AX_READER_SETTINGS* settings_buffer = 
		AX_SAFE_GET_SETTINGS(settings); 
	AXCHARSET* char_set_buffer =
		AX_SAFE_GET_CHAR_SET(settings_buffer, flags);
	uint32_t min_index = 
		AX_SAFE_GET_MIN_INDEX(settings_buffer, flags); 
	uint32_t max_index =
		AX_SAFE_GET_MAX_INDEX(text, settings_buffer, flags); 
	
	/*
	 	Set the start index and char to min_index which can be user defined.

		Example:
			FOR text = L"some text buffer" 
			AND flags |= AX_READ_START_INDEX
			AND settings->min_index = 2
				
			- text_index = 2;
			- text_char = L"me text buffer";
	*/
	uint32_t text_index = min_index;
	const wchar_t* text_char = &text[min_index];	

	// Will indicate if there was any occurrances of the character
	bool found = false;

	/*
		Until text_index is in range between min_index and max_index

		Example:
			FOR text = L"some buffer"
			AND min_index = 2
			AND max_index = 8
			AND flags |= AX_READ_CHAR_SET
			AND char_set_buffer = { L'f', L'\0' }

			Processing will occur only within L"me buff".
			And buffer returned will be L"me bu" as L'f' character is included in char_set_buffer. 
	*/
	while (text_index >= min_index
	&& text_index < max_index){
		// Set found to the result of the check. 
		found = (ax_in_charset(*text_char, *char_set_buffer) == AX_SUCCESS)
			? true
			: false;

		if (found == true){
			break;
		}

		text_char++;
		text_index++;
	}
	
	// If character in the set wasnt found and flags are not forcing return by AX_READ_RETURN
	if (found == false
	&& _ax_check_bit(flags, AX_READ_RETURN) == false){
		return AX_NOT_FOUND;
	}

	/*
		If found the size if equal to the difference between the last processed index and first processed index.

		Example:
			FOR text = L"some buffer"
			AND min_index = 2
			AND text_index = 8

			IF found = true

			some buffer
			  ^   |  	
			 min  |
			      ^
			     text 
			
			This would be resolved as: (7 - 2) = 5 

			IF found = false 

			This would be resolved as: (11 - 1) = 10 (wcslen(text))
	*/
	size_t range_buffer_size = 
		(found == true)
		? text_index - min_index
		: _ax_size_w(text) - 1;

	// Allocate the buffer and add 1 character as null-terminator.
	wchar_t* range_buffer = malloc((range_buffer_size + 1) * sizeof(wchar_t));
	assert(range_buffer != NULL);

	// Copy from original text to the buffer.
	memcpy(range_buffer, &text[min_index], range_buffer_size * sizeof(wchar_t));
	range_buffer[range_buffer_size] = L'\0';
	
	// Write-back the data.
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
	AX_OUT_OPT size_t*			skipped	
){
	if (text == NULL
	|| buffer == NULL){
		return AX_INVALID_ARGUMENT;
	}

	struct AX_READER_SETTINGS* settings_buffer = 
		AX_SAFE_GET_SETTINGS(settings); 
	AXCHARSET* char_set_buffer =
		AX_SAFE_GET_CHAR_SET(settings_buffer, flags);
	uint32_t min_index = 
		AX_SAFE_GET_MIN_INDEX(settings_buffer, flags); 
	uint32_t max_index =
		AX_SAFE_GET_MAX_INDEX(text, settings_buffer, flags); 

	/*
	 	Set the start index and char to min_index which can be user defined.

		Example:
			FOR text = L"some text buffer" 
			AND flags |= AX_READ_START_INDEX
			AND settings->min_index = 2
				
			- text_index = 2;
			- text_char = L"me text buffer";
	*/
	uint32_t text_index = min_index; 
	const wchar_t* text_char = &text[min_index];

	size_t text_size = _ax_size_w(text_char) - 1;
	size_t skip_size = _ax_size_w(skip) - 1;

	if (skip_size > text_size){
		return AX_BUFFER_TOO_BIG;
	}

	bool found = false;

	while (text_index >= min_index
	&& text_index < max_index){
		// Check if char_set_buffer has the current character.
		bool in_charset = (ax_in_charset(*text_char, *char_set_buffer) == AX_SUCCESS);

		// Check if current character extends to the entire skip buffer.	
		found = (ax_compare_range(text_char, skip) == AX_SUCCESS)
			? true
			: false;

		// If is not the charset character AND is found then move to the write-back.
		if (in_charset == false
		&& found == true){
			break;
		}

		text_index++;	
		text_char++;	
	}

	// If character in the set wasnt found and flags are not forcing return by AX_READ_RETURN
	if (found == false
	&& _ax_check_bit(flags, AX_READ_RETURN) == false){
		return AX_NOT_FOUND;
	}

	/*
		If found the size if equal to the difference between the last processed index and first processed index.

		Example:
			FOR text = L"some buffer"
			AND skip = L"me bu"
			AND skip_size = 5
			AND text_index = 2

			IF found = true

			some buffer
			  ^   |
		         text |
			      ^
			     skip
			
			This would be resolved as: &text[2 + 5] = L"ffer"

			IF found = false 
			
			This would be resolved as: &text[0] = L"some buffer" 
	*/
	*buffer = 
		(found == true)
		? &text[text_index + skip_size]
		: text;

	// Size of all skipped characters.
	if (skipped != NULL){
		*skipped = 
			(found == true)
			? skip_size + text_index 
			: 0;
	}

	return (found == true)
		? AX_SUCCESS
		: AX_PARTIAL_ERROR;
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

