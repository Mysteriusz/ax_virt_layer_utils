#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	noded_doc *doc = nullptr; 
	res = noded_load_doc(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));

	res = noded_unload_doc(&doc);
	axcheck(res, ax_log(res));

	unref(res);

	return 0;
}

