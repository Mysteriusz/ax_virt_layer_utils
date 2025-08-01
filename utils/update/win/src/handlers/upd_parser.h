#include "ax_utility.h"

typedef uint8_t UPD_COMMAND_TOKEN_TYPE;
enum{
	SWITCH 		= 0,
	VALUE		= 1,
	FUNCTION	= 2,
};

typedef struct{
	void*			value;
	uint32_t		value_size;
	UPD_COMMAND_TOKEN_TYPE 	token_type;
} UPD_COMMAND_TOKEN;

typedef struct{
	UPD_COMMAND_TOKEN*	tokens;
	uint32_t		token_count;
} UPD_COMMAND_DATA;

AXSTATUS upd_parse(
	AX_IN_OUT UPD_COMMAND_DATA	command_data
);

AXSTATUS upt_execute(
	AX_IN const UPD_COMMAND_TOKEN   token
);

