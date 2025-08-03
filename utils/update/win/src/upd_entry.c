#include "upd_parser.h"
#include "ax_control.h"

int main(){
	AX_DATA_ROOT root;
	ax_get_data_root(&root);

	AX_DATA_NODE* default_data = ax_get_data_node_d();
	for (uint32_t i = 0; i < AX_DATA_NODE_COUNT_D; i++){
		ax_set_data(&root, &default_data[i]);
	}

	ax_free_root(&root);

	ax_control_setup_i();
	ax_driver_setup_i();

	uint32_t arg_count = 0; 
	wchar_t** args = CommandLineToArgvW(GetCommandLineW(), &arg_count);

	// No arguments passed to commandline -> ax_update.exe
	if (arg_count == 1){
		return 0;
	}
	
	ax_log_status(2, 1, _ReturnAddress(), NULL);

	UPD_COMMAND_TOKEN token;
	//upd_token_parse(args[1], wcslen(args[1]), &token);

	getchar();
	return 0;
}

