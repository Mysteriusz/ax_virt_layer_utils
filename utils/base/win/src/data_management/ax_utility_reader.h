#if !defined(AX_UTILITY_READER_INT)
#define AX_UTILITY_READER_INT

#include "ax_utility.h"

typedef wchar_t			AXCHARSET[];

struct AX_READER_SETTINGS {
	wchar_t*  		label;
	AXCHARSET* 		char_set;	
};

#define AX_DEFAULT_READER_SETTINGS (struct AX_READER_SETTINGS){ \
	.label = NULL, \
	.char_set = {0} \
}

#define AX_DEFAULT_CHAR_SET (AXCHARSET(*))(&(AXCHARSET){ \
	L'\x20', \
	L'\n', \
	L'\r', \
	L'\t', \
	L'\0' \
})
#define AX_PATH_CHAR_SET (AXCHARSET(*))(&(AXCHARSET){ \
	L'\\', \
	L'/', \
	L':', \
	L'\0' \
})

#define AX_SAFE_GET_SETTINGS(settings_ptr) ( \
	(struct AX_READER_SETTINGS*) \
	((settings_ptr != NULL) \
	? settings_ptr \
	: &AX_DEFAULT_READER_SETTINGS) \
)
#define AX_SAFE_GET_CHAR_SET(settings_ptr) ( \
	(AXCHARSET*) \
	((settings_ptr->char_set != NULL) \
	? settings_ptr->char_set \
	: AX_DEFAULT_CHAR_SET) \
)

AXSTATUS ax_read_range(
	AX_IN const wchar_t*			text,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t**			buffer,
	AX_OUT size_t*				buffer_size
);

AXSTATUS ax_skip_text(
	AX_IN_OUT const wchar_t**		text,
	AX_IN wchar_t*				skip
);
AXSTATUS ax_skip_span(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set, 
	AX_OUT size_t*				span
);
AXSTATUS ax_skip_set(
	AX_IN_OUT const wchar_t**		text,
	AX_IN AXCHARSET				char_set 
);

AXSTATUS ax_char_count(
	AX_IN const wchar_t*			text,
	AX_IN AXCHARSET				char_set,
	AX_OUT size_t* 				count
);

AXSTATUS ax_split_text(
	AX_IN const wchar_t*			text,
	AX_IN_OPT struct AX_READER_SETTINGS*	settings,
	AX_OUT wchar_t***			buffer,
	AX_OUT size_t*				buffer_size
);

#endif // !defined(AX_UTILITY_READER_INT)

