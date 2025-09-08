#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;
	io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res);

	u64 off = 0;
	res = skip_while_f(&file, CHARSET_WS, &off);
	axcheck(res, ax_log(res));

	c16 *t = axmalloc(100);
	res = io_fr(&file, 100, t, nullptr); 
	axcheck(res, ax_log(res));
	io_str(t);

	return 0;
}

