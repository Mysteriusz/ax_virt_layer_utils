#include "install.h"

int main(){
   	int arg_count = 0;
    	wchar_t** arg_list = CommandLineToArgvW(GetCommandLineW(), &arg_count);

	// TODO: Make arguments that accept custom AX_DATA_NODE key creation and selection
	for (int i = 0; i < arg_count; i++){
		if (wcscmp(arg_list[i], L"-install") == 0){
			AXSTATUS res = ax_setup_i(NULL, 0);
			if (AX_ERROR(res)) ax_print_error(res, (void*)ax_setup_i);
		}
	}

	while(1);	
    	return 0;
}
