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

	res = find_sequence(L"some_text|other_text", L"%s|%s");
	axcheck(res, ax_log(res));

	return 0;
}

