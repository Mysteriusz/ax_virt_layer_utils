#if !defined(UPD_PARSER_INT)
#define UPD_PARSER_INT

#include "upd_utility.h"

AXSTATUS upd_command_parse(
	AX_IN const wchar_t** 		args,
	AX_IN uint32_t			arg_count,
	AX_OUT UPD_COMMAND**		command
);

AXSTATUS upd_token_parse(
	AX_IN const wchar_t* 		value,
	AX_OUT UPD_COMMAND_TOKEN**	token
);

void upd_command_free(
	AX_IN_OUT UPD_COMMAND*		command
);
void upd_token_free(
	AX_IN_OUT UPD_COMMAND_TOKEN*	token
);

#define UPD_EMPTY_SKIP_SET (const wchar_t[]){ \
	L'\x0009', \
	L'\x000a', \
	L'\x000b', \
	L'\x000c', \
	L'\x000d', \
	L'\x0020', \
	L'\x00a0', \
	L'\0'  \
}

// (
#define UPD_TOKEN_START_SKIP_SET (const wchar_t[]){ \
	L'\x0028', \
	L'\0'  \
}

// )
#define UPD_TOKEN_END_SKIP_SET (const wchar_t[]){ \
	L'\x0029', \
	L'\0'  \
}

// <
#define UPD_EXPRESSION_START_SKIP_SET (const wchar_t[]){ \
	L'\x003c', \
	L'\0'  \
}
// >
#define UPD_EXPRESSION_END_SKIP_SET (const wchar_t[]){ \
	L'\x003e', \
	L'\0'  \
}

// -
#define UPD_SWITCH_START_SKIP_SET (const wchar_t[]){ \
	L'\x002d', \
	L'\0'  \
}
#define UPD_SWITCH_END_SKIP_SET (const wchar_t[]){ \
	L'\x0020', \
	L'\n', \
	L'\r', \
	L'\0'  \
}

// Lower 8 bits of the skip flag are reserved for the value.
// Higher 8 bits of the skip flag are reserved for the bit flags

#define UPD_SKIP_FLAG_LOWER(skip_flag)  (skip_flag & 0x00ff)
#define UPD_SKIP_FLAG_UPPER(skip_flag) 	(skip_flag & 0xff00)

#define UPD_SKIP_FLAG_FO 		0x0000 // Return on first occurence
#define UPD_SKIP_FLAG_LO 		0x0001 // Return on last occurence
#define UPD_SKIP_FLAG_ALL		0x0002 // Return after skipping all characters in the skip_set
#define UPD_SKIP_FLAG_ALWAYS_RET	0x8000 // Specify if value should point back at the begining if not found

wchar_t* upd_range(
	AX_IN const wchar_t*		from,
	AX_IN const wchar_t 		skip_set[], // Array of characters that stop range lookup
	AX_IN uint16_t 			skip_flag,
	AX_OUT size_t*			range_size 			
);
const wchar_t* upd_skip(
	AX_IN const wchar_t*		string,
	AX_IN const wchar_t 		skip_set[], // Array of characters to skip
	AX_IN uint16_t 			skip_flag
);

#endif // !defined(UPD_PARSER_INT)

