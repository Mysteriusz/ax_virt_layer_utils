#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;
	io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res);

	/*u64 s = 0;
	res = trim(L"  some_text  ", L" ", &s, nullptr);
	c16 *b = axmalloc(s * sizeof(c16));
	res = trim(L"  some_text  ", L" ", &s, b);
	axcheck(res, ax_log(res));*/
	u64 s = 0;
	c16 *occ = nullptr;
	res = read_until(L"someotherword", L"d", &s, occ);
	occ = axmalloc(s * sizeof(c16));
	res = read_until(L"someotherword", L"d", &s, occ);
	axcheck(res, ax_log(res));

	io_str(occ);
	io_str_b(occ);
 	ax_log(res);

	/*u64 off = 0;
	//res = skip_until_f(&file, L"f", &off);
	res = skip_while_f(&file, L"[", &off);
	axcheck(res, ax_log(res));

	c16 *t = axmalloc(100);
	res = io_fr(&file, 100, t, nullptr); 
	axcheck(res, ax_log(res));
	io_str(t);*/

	return 0;
}

