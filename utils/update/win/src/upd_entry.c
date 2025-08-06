#include "upd_parser.h"
#include "upd_executor.h"
#include "ax_control.h"

int main(){
	int arg_count = 0; 
	wchar_t** args = CommandLineToArgvW(GetCommandLineW(), &arg_count);

	// No arguments passed to commandline -> ax_update.exe
	if (arg_count == 1){
		return 0;
	}

	UPD_COMMAND* command = NULL;
	upd_command_parse((const wchar_t**)args, arg_count, &command);
	upd_execute_command(command, 1, NULL, NULL);

	getchar();
	return 0;
}

