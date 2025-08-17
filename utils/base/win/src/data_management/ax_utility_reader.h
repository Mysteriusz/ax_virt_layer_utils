#if !defined(AX_UTILITY_READER_INT)
#define AX_UTILITY_READER_INT

#include "ax_utility.h"

typedef wchar_t			AXCHARSET[];

typedef uint64_t		AXREADFLAG;
typedef uint8_t			AXREADMODE;

/*
 	Function flow flags.

*/

#define AX_READ_MODE_FIRST(mode) \
	(mode == AX_READ_BEFORE_FIRST \
	|| mode == AX_READ_AFTER_FIRST \
	|| mode == AX_READ_AT_FIRST)
#define AX_READ_MODE_LAST(mode) \
	(mode == AX_READ_BEFORE_LAST \
	|| mode == AX_READ_AFTER_LAST \
	|| mode == AX_READ_AT_LAST)

#define AX_READ_MODE_BEFORE(mode) \
	(mode == AX_READ_BEFORE_FIRST \
	|| mode == AX_READ_BEFORE_LAST)
#define AX_READ_MODE_AFTER(mode) \
	(mode == AX_READ_AFTER_FIRST \
	|| mode == AX_READ_AFTER_LAST)
#define AX_READ_MODE_AT(mode) \
	(mode == AX_READ_AT_FIRST \
	|| mode == AX_READ_AT_LAST)

#define AX_READ_MODE_INVALID(mode) \
	(mode == 0 \
	|| mode > UINT8_MAX)

/*
 
 	Return before the first occurrence.

	Example:
		- other<<some>data>>>>other
		      ^
		     ret	

*/
#define AX_READ_BEFORE_FIRST	0x0000000000000001

/*
 
 	Return before the last occurrence.

	Example:
		- other<<some>data>>>>other
				 ^
				ret	

*/
#define AX_READ_BEFORE_LAST 	0x0000000000000002

/*
 
 	Return after the first occurrence.

	Example:
		- other<<some>data>>>>other
		         ^
		        ret	

*/
#define AX_READ_AFTER_FIRST	0x0000000000000003

/*
 
 	Return after the last occurrence.

	Example:
		- other<<some>data>>>>other
				      ^
				     ret	

*/
#define AX_READ_AFTER_LAST 	0x0000000000000004

/*
 
 	Return at the first occurrence.

	Example:
		- other<<some>data>>>>other
		       ^
		      ret	

*/
#define AX_READ_AT_FIRST	0x0000000000000005

/*
 
 	Return at the last occurrence.

	Example:
		- other<<some>data>>>>other
				  ^
				 ret	

*/
#define AX_READ_AT_LAST 	0x0000000000000006

/*

	AX Universal function control flags. 

*/

/*
 
 	Return no matter if there was an occurrence.
	In that case function has to return at the begining of the buffer.

	Example:
		- other some data other
		  ^
	         ret	

	This will return AXSTATUS of value AX_PARTIAL_ERROR instead of AX_NOT_FOUND.

*/
#define AX_READ_RETURN		0x0000000000000100

/*
 
   	AX data flags
	Be cautious when using any of the data flag since they ASSERT that the property is not NULL.

*/

/*
 
 	Indicates that label has to be first skipped and ASSERTS it as not null.

*/
#define AX_READ_LABEL		0x8000000000000000

/*
 
 	Indicates that charset has to be used with all operations.

*/
#define AX_READ_CHAR_SET	0x4000000000000000

/*
 
 	Indicates that charset has to be used with all operations.
	If not set the start index is 0.


*/
#define AX_READ_START_INDEX	0x2000000000000000

/*
 
 	Indicates buffer index which ends the read.
	If not set the end index is the first \0 character.

*/
#define AX_READ_END_INDEX	0x1000000000000000

struct AX_READER_SETTINGS {
	wchar_t*  		label;
	AXCHARSET* 		char_set;	
	uint32_t		start_index;
	uint32_t		end_index;
};

#define AX_DEFAULT_CHAR_SET (AXCHARSET(*))(&(AXCHARSET){ \
	L'\0' \
})
#define AX_PATH_CHAR_SET (AXCHARSET(*))(&(AXCHARSET){ \
	L'\\', \
	L'/', \
	L':', \
	L'\0' \
})

#define AX_DEFAULT_READER_SETTINGS (struct AX_READER_SETTINGS){ \
	.label = NULL, \
	.char_set = AX_DEFAULT_CHAR_SET, \
	.start_index = 0, \
	.end_index = 0, \
}

#define AX_SAFE_GET_SETTINGS(settings_ptr) ( \
	(struct AX_READER_SETTINGS*) \
	((settings_ptr != NULL) \
	? settings_ptr \
	: &AX_DEFAULT_READER_SETTINGS) \
)

#define AX_SAFE_GET_MIN_INDEX(settings_ptr, flags) ( \
	(_ax_check_bit(flags, AX_READ_START_INDEX) == true) \
	? (settings_ptr)->start_index \
	: 0 \
)

#define AX_SAFE_GET_MAX_INDEX(text, settings_ptr, flags) ( \
	(_ax_check_bit(flags, AX_READ_END_INDEX) == true) \
	? settings_ptr->end_index \
	: (uint32_t)_ax_size_w(text) \
)

#define AX_SAFE_GET_CHAR_SET(settings_ptr, flags) ( \
	(_ax_check_bit(flags, AX_READ_CHAR_SET) == true) \
	? settings_ptr->char_set \
	: AX_DEFAULT_CHAR_SET \
)


AXSTATUS ax_read_range(
	AX_IN const wchar_t*			text,
	AX_IN AXREADFLAG			flags,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t**			buffer,
	AX_OUT size_t*				buffer_size
);

AXSTATUS ax_skip_range(
	AX_IN const wchar_t*			text,
	AX_IN const wchar_t*			skip,
	AX_IN AXREADFLAG			flags,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT const wchar_t**			buffer,
	AX_OUT_OPT size_t*			skipped	
);
AXSTATUS ax_skip_charset(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set,
	AX_OUT const wchar_t**			buffer,
	AX_OUT_OPT size_t*			skipped
);

AXSTATUS ax_mode_offset(
	AX_IN uint32_t				start_index,
	AX_IN AXREADMODE			mode,
	AX_OUT int32_t* 			index_offset
);
AXSTATUS ax_compare_range(
	AX_IN const wchar_t*			text,
	AX_IN const wchar_t*			word
);
AXSTATUS ax_in_charset(
	AX_IN const wchar_t			value,
	AX_IN AXCHARSET				char_set
);

#endif // !defined(AX_UTILITY_READER_INT)

