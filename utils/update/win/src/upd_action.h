#include "ax_utility.h"

// Control and driver interfaces
#include "ax_control.h"
#include "ax_driver.h"

typedef AXSTATUS(*UPD_ACTION)(void* stack);

struct UPD_INSTALL_STACK{
	AX_DATA_TYPE __data_type; // --data_type= ? directory : file : registry	
	wchar_t* __path; // --path= ?
};

#define UPD_ACTION_INSTALL_STACK_SIZE	0 //sizeof(struct UPD_INSTALL_STACK)	
AXSTATUS upd_action_install(
	AX_IN_OPT struct UPD_INSTALL_STACK*	stack
);
#define UPD_ACTION_UPDATE_STACK_SIZE	0x00000000
AXSTATUS upd_action_update(
	AX_IN_OPT void*			stack
);

