#include "ax_utility.h"

typedef AXSTATUS(*UPD_ACTION)(void* stack);

#define UPD_ACTION_INSTALL_STACK_SIZE	0x00000000
AXSTATUS upd_action_install(
	AX_IN_OPT void*			stack
);
#define UPD_ACTION_UPDATE_STACK_SIZE	0x00000000
AXSTATUS upd_action_update(
	AX_IN_OPT void*			stack
);

