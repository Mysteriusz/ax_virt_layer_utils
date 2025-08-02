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

#define UPD_EMPTY_SKIP_SET (const wchar_t[]){ \
	L'\x0009', \
	L'\x000a', \
	L'\x000b', \
	L'\x000c', \
	L'\x000d', \
	L'\x0020', \
	L'\x00a0', \
	L'\x0000'  \
}
#define UPD_TOKEN_START_SKIP_SET (const wchar_t[]){ \
	L'\x0028', \
	L'\x0000'  \
}
#define UPD_TOKEN_END_SKIP_SET (const wchar_t[]){ \
	L'\x0029', \
	L'\x0000'  \
}

#define UPD_EXPRESSION_START_SKIP_SET (const wchar_t[]){ \
	L'\x003c', \
	L'\x0000'  \
}
#define UPD_EXPRESSION_END_SKIP_SET (const wchar_t[]){ \
	L'\x003e', \
	L'\x0000'  \
}

const wchar_t* upd_skip(
	AX_IN const wchar_t*		string,
	AX_IN const wchar_t 		skip_set[] // Array of characters to skip
);

void upd_command_free(
	AX_IN_OUT UPD_COMMAND*		command
);
void upd_token_free(
	AX_IN_OUT UPD_COMMAND_TOKEN*	token
);

