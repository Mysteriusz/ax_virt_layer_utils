#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	io_str(u"ax_update.exe v0.00");

	axres res = AX_SUCC;
	unref(res);

	noded_doc *doc = nullptr;
	res = noded_doc_load(u"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));
	noded_doc_unload(doc);

	/*seq_loc loc = {0};
	ax_dict_init(3, &loc.seq_vars);
	res = seq_find(u"node_str=", NODED_KVP_FMT, &loc);

	io_str(index_as(loc.seq_vars, c16*, u"kvp_name", sizeof(u"kvp_name")));*/

	/*ax_list *locs = nullptr;
	ax_list_init(&locs);

	res = seq_find_all(u"[sect_1]:=other_t[sect_21]:ext-", u"\\[[n:name;s:1]<{a-z}+{_}+{0-9}>]:", true, locs);
	axcheck(res, ax_log(res));

	seq_loc *loc = index_as(locs, seq_loc*, 1);
	io_str(index_as(loc->seq_vars, c16*, u"name", sizeof(u"name")));

	locs->delete(locs);*/

	/*io_i64(loc.seq_vars->count);
	io_str(index_as(loc.seq_vars, c16*, u"name", sizeof(u"name")));

	loc.seq_vars->delete(loc.seq_vars);*/

	/*seq_loc loc = {0};

	res = seq_find(u"[sh(abcdsdfsjdsf-", u"^\\[sh\\(abc$", &loc);
	axcheck(res, ax_log(res));

	io_str(loc.beg);
	io_str(loc.end);*/
	/*const c16 *fmt = u"sdkalj\\[sh";
	u32 s = 0;
	io_str(_seq_read_range(fmt, fmt, &s));*/

	//io_str(index_as(dict, c16*, u"name", sizeof(u"name")));

	/*io_str(loc.beg);
	io_str(loc.end);*/

	//io_i64(seq_label_to_var_inv(u"n:key;t:u32"));
	//io_i64(seq_label_to_var_inv(u"n:key;t:u32"));

	//io_i64(murmur_x64_128((const u8*)"abc", 4, 0));

	io_str(u"Active memory in bytes:");
	io_i64(_MEM_ACTIVE);

	axcheck(res, ax_log(res));

	return 0;
}

