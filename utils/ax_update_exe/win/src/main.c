#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	io_str(L"ax_update v0.00");

	axres res = AX_SUCC;

	noded_doc *doc = nullptr;

	res = noded_doc_load(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));

	noded_doc_unload(doc);

	io_str(L"Active memory in bytes:");
	io_i64(_MEM_ACTIVE);

	axcheck(res, ax_log(res));
	unref(res);

	return 0;
}

