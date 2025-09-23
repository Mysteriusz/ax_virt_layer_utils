#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	/*io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res, ax_log(res));

	res = seq_find_f(&file, L"\\[<.>|<a-z>]:\\");
	axcheck(res, ax_log(res));

	c16 *buf = axmalloc(100);
	res = io_fr(&file, 100, buf, nullptr); 
	axcheck(res, ax_log(res));
	io_str(buf);

	io_fc(&file);*/
	unref(res);

	const c16 *seq = L"\\[<a-z>|<.>]:\\";
	const c16 *txt = L"l[sect[ion|other[abc|_text]:|a|b]:";
	//const c16 *loc = nullptr;

	ax_list *occs = nullptr;
	ax_list_init(&occs);

	res = seq_find_all(txt, seq, occs);
	axcheck(res, ax_log(res));

	io_str(*i_as(occs, 0, c16**));
	io_str(*i_as(occs, 1, c16**));
	io_i64(occs->count);

	return 0;
}

