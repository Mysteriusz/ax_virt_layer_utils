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

	ax_list *list = nullptr;
	u32 fmt_c = 0;
	res = seq_split_fmt(L"\\[<.>]:\\[[[<.>]:\\", &fmt_c, &list);
	axcheck(res, ax_log(res));

	fmt_group *a = i_as(list, 1, fmt_group*);
	io_str(i_as(a->seq_list, 0, c16*));

	return 0;
}

