#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	io_file file = {0};

	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file); 
	axcheck(res, ax_log(res));

	const c16 *seq = L"\\[<a-z>|<.>]:\\";

	ax_list *list = nullptr;
	ax_list_init(&list);

	res = seq_find_all_f(&file, seq, list);
	axcheck(res, ax_log(res));

	io_i64(list->count);
	io_str(*i_as(list, 0, c16**));

	io_fc(&file);
	unref(res);

	return 0;
}

