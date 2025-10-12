#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	/*noded_doc *doc = nullptr;
	res = noded_doc_load(L"D:\\share\\ax_info.noded", &doc);
	noded_doc_unload(doc);*/

	/*io_file *file = nullptr;
	io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file);
	seq_find_f(file, L"\\(0->'$')[sect b]:\\");
	io_fc(file);*/

	io_str(L"fds");
	io_i64(seq_func_to_cond_inv(L"(!:[$])"));

	/*seq_loc loc = {0};
	seq_find(L"adhfa[sec]:jdskejsl", L"\\(!->[$])[<{a-z}>]:\\", &loc);
	io_str(loc.beg);*/

	io_i64(_MEM_ACTIVE);
	axcheck(res, ax_log(res));
	unref(res);

	return 0;
}

