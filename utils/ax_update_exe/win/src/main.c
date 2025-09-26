#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	/*noded_doc *doc = nullptr; 
	res = noded_init_doc(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));*/

	/*const c16 *loc = nullptr;
	res = seq_find(L"lsl[my-section]abcd", L"\\[<{a-z}+{-}>]\\", &loc);
	axcheck(res, ax_log(res));

	io_str(loc);*/
	ax_list *g = nullptr;
	res = seq_split_fmt(L"\\[<{a-z}+{_}>|<{a-b}>]\\", &g);
	axcheck(res, ax_log(res));
	fmt_group *grp = index_as(g, 0, fmt_group*);
	unref(grp);

	//io_i64(seq_cap_to_charset_inv(L"{a-}"));

	unref(res);

	return 0;
}

