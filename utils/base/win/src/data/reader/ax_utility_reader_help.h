#if !defined(ax_utility_reader_help_int)
#define AX_UTILITY_READER_HELP_INT

#include "ax_utility_reader.h"

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

#endif // !defined(AX_UTILITY_READER_HELP_INT)

