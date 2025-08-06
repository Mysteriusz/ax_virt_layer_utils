#if !defined(UPD_UTILITY_INT)
#define UPD_UTILITY_INT

#include "ax_utility.h"
#include "upd_action.h"

typedef uint8_t UPD_COMMAND_TOKEN_TYPE;
enum{
	VALUE = 0,
	SWITCH = 1,
	EXPRESSION = 2,
};

typedef struct{
	void*			value;
	size_t 			value_size;
	UPD_COMMAND_TOKEN_TYPE 	token_type;
} UPD_COMMAND_TOKEN;

#define UPD_SWITCH_ALL			0xffffffffffffffff
#define UPD_SWITCH_PRIORITY_LOW		0x0000000000000001
#define UPD_SWITCH_PRIORITY_NORMAL	0x0000000000000002
#define UPD_SWITCH_PRIORITY_MEDIUM	0x0000000000000004
#define UPD_SWITCH_PRIORITY_HIGH	0x0000000000000008
#define UPD_SWITCH_PRIORITY_VERY_HIGH	0x0000000000000010
#define UPD_SWITCH_PRIORITY_MAIN	0x0000000000000020

#define UPD_SWITCH_ACTION_STACK		0x0000000100000000
typedef struct{
	UPD_COMMAND_TOKEN**	tokens;
	uint32_t		token_count;
} UPD_COMMAND;

typedef struct {
	wchar_t*		switch_string;
	uint64_t		exclusion_flags; // Tells what other switch flags are not able to execute with this switch	
	uint64_t		switch_flags; // Tells what flags this switch has
	size_t			stack_size; // if switch_flags & UPD_SWITCH_ACTION_STACK
	UPD_ACTION		action;
} UPD_SWITCH_DESCRIPTOR;

static const UPD_SWITCH_DESCRIPTOR UPD_SWITCH_TABLE[] = { \
	{ \
		.switch_string = 	L"install", \
		.exclusion_flags = 	UPD_SWITCH_ALL, \
		.switch_flags = 	UPD_SWITCH_PRIORITY_MAIN, \
		.stack_size = 		UPD_ACTION_UPDATE_STACK_SIZE, \
		.action = 		upd_action_install, \
	}, \
	{ \
		.switch_string = 	L"update", \
		.exclusion_flags = 	UPD_SWITCH_ALL, \
		.switch_flags = 	UPD_SWITCH_PRIORITY_MAIN, \
		.stack_size = 		UPD_ACTION_UPDATE_STACK_SIZE, \
		.action = 		upd_action_update, \
	}, \
};
#define UPD_SWITCH_TABLE_COUNT	 	(sizeof(UPD_SWITCH_TABLE) / sizeof(UPD_SWITCH_DESCRIPTOR))

#endif // !defined(UPD_UTILITY_INT)

