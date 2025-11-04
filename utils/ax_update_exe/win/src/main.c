#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	io_str(u"ax_update.exe v0.00");

	axres res = AX_SUCC;
	unref(res);

	/*noded_doc *doc = nullptr;
	res = noded_doc_load(u"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));
	noded_doc_unload(doc);*/

	/*u32 l = 0;
	c16 *b = nullptr;
	read_until(u"some_text;dfhsklh", u";", &l, b);
	b = axmalloc(l * sizeof(c16));
	read_until(u"some_text;dfhsklh", u";", &l, b);

	io_str(b);*/
	
	seq_loc loc = {0};
	res = seq_find(u"df=other_text-", u"[n:name;s:3]^?<{a-z}+{_}>=<{a-z}+{_}>$", &loc);
	axcheck(res, ax_log(res));
	/*io_str(loc.beg);
	io_str(loc.end);*/

	//io_i64(seq_label_to_var_inv(u"n:key;t:u32"));
	//io_i64(seq_label_to_var_inv(u"n:key;t:u32"));

	io_str(u"Active memory in bytes:");
	io_i64(_MEM_ACTIVE);

	axcheck(res, ax_log(res));

	return 0;
}

