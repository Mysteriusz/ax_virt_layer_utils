#if !defined(AX_UTILITY_READER_INT)
#define AX_UTILITY_READER_INT

#include "ax_utility.h"

typedef wchar_t			AXSKIPSET[];

struct AX_READER_SETTINGS {
	wchar_t*  		label;
	AXSKIPSET* 		skip_set;	
};

#define AX_DEFAULT_READER_SETTINGS (struct AX_READER_SETTINGS){ \
	.label = NULL, \
	.skip_set = {0} \
}

#define AX_DEFAULT_SKIP_SET (AXSKIPSET){ \
	L'\x20', \
	L'\n', \
	L'\r', \
	L'\t', \
	L'\0' \
}

#define AX_SAFE_GET_SETTINGS(settings_ptr) ( \
	(struct AX_READER_SETTINGS*) \
	((settings_ptr != NULL) \
	? settings_ptr \
	: &AX_DEFAULT_READER_SETTINGS) \
)
#define AX_SAFE_GET_SKIP_SET(settings_ptr) ( \
	(AXSKIPSET*) \
	((settings_ptr->skip_set != NULL) \
	? settings_ptr->skip_set \
	: (AXSKIPSET(*))&AX_DEFAULT_SKIP_SET) \
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
	AX_IN AXSKIPSET				skip_set, 
	AX_OUT size_t*				span
);
AXSTATUS ax_skip_set(
	AX_IN_OUT const wchar_t**		text,
	AX_IN AXSKIPSET				skip_set 
);

#endif // !defined(AX_UTILITY_READER_INT)

