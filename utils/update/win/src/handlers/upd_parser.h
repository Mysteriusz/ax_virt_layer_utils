#include "ax_utility.h"

typedef uint8_t UPD_COMMAND_TOKEN_TYPE;
enum{
	SWITCH = 0,
	VALUE = 1,
	EXPRESSION = 2,
};

typedef struct{
	void*			value;
	size_t 			value_size;
	UPD_COMMAND_TOKEN_TYPE 	token_type;
} UPD_COMMAND_TOKEN;

typedef struct{
	UPD_COMMAND_TOKEN*	tokens;
	uint32_t		token_count;
} UPD_COMMAND;

AXSTATUS upd_command_parse(
	AX_IN const wchar_t** 		args,
	AX_IN const uint32_t		arg_count,
	AX_OUT UPD_COMMAND*		command
);

#define UPD_ALLOWED_TOKEN_COUNT
#define UPD_ALLOWED_TOKENS {L"--install", L"--update"} 
AXSTATUS upd_token_parse(
	AX_IN const wchar_t* 		value,
	AX_IN const uint32_t		value_length,
	AX_OUT UPD_COMMAND_TOKEN*	token
);

AXSTATUS upd_execute_expression(
	AX_IN const wchar_t* 		expression,
	AX_OUT size_t*			buffer_size,
	AX_OUT void**			buffer
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
#define UPD_TOKEN_START_SKIP_SET (const wchar_t[]){ \
	L'\x0028', \
	L'\0'  \
}
#define UPD_TOKEN_END_SKIP_SET (const wchar_t[]){ \
	L'\x0029', \
	L'\0'  \
}

#define UPD_EXPRESSION_START_SKIP_SET (const wchar_t[]){ \
	L'\x003c', \
	L'\0'  \
}
#define UPD_EXPRESSION_END_SKIP_SET (const wchar_t[]){ \
	L'\x003e', \
	L'\0'  \
}

#define UPD_SKIP_FLAG_FO 			0x0000 // Return on first occurence
#define UPD_SKIP_FLAG_LO 			0x0001 // Return on last occurence
#define UPD_SKIP_FLAG_ALL			0x0002 // Return after skipping all characters in the skip_set

const wchar_t* upd_skip(
	AX_IN const wchar_t*		string,
	AX_IN const wchar_t 		skip_set[], // Array of characters to skip
	AX_IN uint16_t 			skip_flag
);

void upd_command_free(
	AX_IN_OUT UPD_COMMAND*		command
);
void upd_token_free(
	AX_IN_OUT UPD_COMMAND_TOKEN*	token
);

