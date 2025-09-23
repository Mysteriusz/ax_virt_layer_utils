#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;
	io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res, ax_log(res));

	res = seq_find_f(&file, L"\\[<.>|<.>]:\\");
	axcheck(res, ax_log(res));

	c16 *buf = axmalloc(100);
	res = io_fr(&file, 100, buf, nullptr); 
	axcheck(res, ax_log(res));
	io_str(buf);

	io_fc(&file);
	unref(res);

	/*const c16 *seq = L"\\[<a-z>|<.>]:\\";
	const c16 *txt = L"l[sect[ion|other_text|a|b]:";
	const c16 *loc = nullptr;

	res = seq_find(txt, seq, &loc);
	axcheck(res, ax_log(res));

	io_str(loc);*/

	return 0;
}

