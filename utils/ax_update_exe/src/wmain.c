#include "argv.h"

int wmain(
	i32		argc,
	c16 		**argv
){
	io_str(u"ax_update.exe v0.00");

	axres res = AX_SUCC;
	unref(res);

	if (check_argv(argv, argc)){
		io_str(u"Error when parsing arguments.");
		return -1;
	}

	axcheck(res, ax_log(res));

	return 0;
}

