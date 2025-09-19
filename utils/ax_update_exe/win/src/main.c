#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;
	/*io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res, ax_log(res));

	res = find_substr_f(&file, L"my_config_value_some_other_value");
	axcheck(res, ax_log(res));
	c16 *buf = axmalloc(100);
	u64 read = 0;
	res = io_fr(&file, 100, buf, &read); 
	axcheck(res, ax_log(res));
	io_str(buf);

	io_fc(&file);*/
	unref(res);

	const c16 *seq = L"\\[<a-z>|<.>]\\";
	const c16 *txt = L"l[sect[ion|other_text|a|b]:";

	ax_list *list = nullptr;
	u32 fmt_c = 0;
	res = seq_split_fmt(seq, &fmt_c, &list);
	axcheck(res, ax_log(res));

	/*fmt_group *root = i_as(list, 0, fmt_group*);
	res = seq_match(txt, &txt[2], &txt[9], i_as(root->cap_sets, 0, c16*));
	axcheck(res, ax_log(res));*/

	const c16 *loc = nullptr;
	res = seq_locate(txt, i_as(list, 0, fmt_group*), &loc); 
	axcheck(res, ax_log(res));
	io_str(loc);

	return 0;
}

