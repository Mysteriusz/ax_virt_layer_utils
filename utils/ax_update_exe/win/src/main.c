#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	seq_loc loc = {0};
	res = seq_find(L"some_text[data]:\td_text", L"\\<{a-z}+{_}>[<{a-z}>]:\t<{a-z}>\\", &loc);
	axcheck(res, ax_log(res));

	io_str(loc.beg);
	io_str(loc.end);

	/*ax_list *grp_list = nullptr;
	res = seq_split_fmt(L"\\<{-}>[<{a-z}>]\n\\", &grp_list);
	axcheck(res, ax_log(res));
	fmt_group *grp = index_as(grp_list, 0, fmt_group*);

	io_str(index_as(grp->spec_list, 0, fmt_spec*)->value);
	io_str(index_as(grp->spec_list, 1, fmt_spec*)->value);
	io_str(index_as(grp->spec_list, 2, fmt_spec*)->value);
	io_str(index_as(grp->spec_list, 3, fmt_spec*)->value);
	io_i64(grp->spec_list->count);*/
	

	/*noded_doc *doc = nullptr; 
	res = noded_load_doc(L"D:\\share\\ax_info.noded", &doc);
	axcheck(res, ax_log(res));

	noded_sect *sect = nullptr;
	res = noded_load_sect(doc, L"secta", &sect);
	axcheck(res, ax_log(res));

	noded_kvp *kvp = nullptr;
	res = noded_load_kvp(sect, L"node1", &kvp);
	axcheck(res, ax_log(res));*/

	/*res = noded_unload_doc(&doc);
	axcheck(res, ax_log(res));*/

	unref(res);

	return 0;
}

