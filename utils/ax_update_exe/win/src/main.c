#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	noded_doc *doc = nullptr; 
	res = noded_init_doc(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));

	/*u64 len = 0;
	c16 *buf = nullptr;

	res = c16_difference(L"abcdefghijk", L"cdgh", &len, buf);
	axcheck(res, ax_log(res));

	buf = axmalloc(len * sizeof(c16));

	res = c16_difference(L"abcdefghijk", L"cdgh", &len, buf);
	axcheck(res, ax_log(res));

	io_str_b(buf);*/

	unref(res);

	return 0;
}

