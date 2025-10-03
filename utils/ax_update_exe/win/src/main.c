#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	io_i64(_MEM_ACTIVE);
	axcheck(res, ax_log(res));

	/*io_file file = {0};
	res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file);
	axcheck(res, ax_log(res));
	res = seq_find_f(&file, L"\\<{[}>[<{a-z}>]:\\");
	axcheck(res, ax_log(res));*/

	/*noded_doc *doc = nullptr; 
	res = noded_load_doc(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));*/

	/*noded_sect *sect = nullptr;
	res = noded_load_sect(doc, L"secta", &sect);
	axcheck(res, ax_log(res));

	noded_kvp *kvp = nullptr;
	res = noded_load_kvp(sect, L"node1", &kvp);
	axcheck(res, ax_log(res));*/

	unref(res);

	return 0;
}

