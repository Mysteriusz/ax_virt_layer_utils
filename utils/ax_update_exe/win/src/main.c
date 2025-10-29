#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	io_str(L"ax_update.exe v0.00");

	axres res = AX_SUCC;
	unref(res);

	/*noded_doc *doc = nullptr;
	res = noded_doc_load(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));
	noded_doc_unload(doc);*/

	seq_loc loc = {0};
	res = seq_find(L"bef|defl]aft", L"|?<{a-c}>?<{d-f}>]", &loc);
	axcheck(res, ax_log(res));

	io_str(loc.beg);

	io_str(L"Active memory in bytes:");
	io_i64(_MEM_ACTIVE);

	axcheck(res, ax_log(res));

	return 0;
}

