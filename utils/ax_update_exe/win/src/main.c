#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;


	seq_loc loc = {0};
	//res = seq_find(L"spmedfjkljds[sec]::dfsjdfks", L"[<{a-z}>]:", &loc);
	res = seq_find(L"l[^section|other_text||a|b]:", L"[\\^<{a-z}>|<{.}>]:", &loc);
	ax_log(res);
	io_str(loc.beg);
	io_str(loc.end);

	/*fmt_group grp = {0};
	ax_list_init(&grp.spec_list);
	ax_list_init(&grp.cond_list);

	seq_read_group(L"[^df<{a-z}>|$<{.}>]:", &grp);
	io_str(index_as(grp.spec_list, 0, c16*));*/

	/*const c16 *fmt = L"[<{a-z}\\{>+fdsjkfdsjjjjj\\>]<{:}:"; 
	const c16 *loc = nullptr;
	ax_log(seq_group_cap_end(fmt, &fmt[1], &loc));
	io_str(loc);*/

	io_i64(_MEM_ACTIVE);
	axcheck(res, ax_log(res));
	unref(res);

	return 0;
}

