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

	/*fmt_cond *cond = seq_func_to_cond(L"(!:`[$]`)"); 
	io_i64((cond == nullptr));
	axcheck((cond == nullptr));

	io_str(cond->bef);
	io_str(cond->aft);
	io_i64(cond->mode);*/
	/*const c16 *fmt = L"(!:`\"$\"`)[\x2<{a-z}+{[-]}>\x3]";
	ax_list *spec_list = nullptr;
	ax_list_init(&spec_list);

	const c16 *loc = nullptr;
	res = seq_group_condition(fmt, &fmt[0], spec_list, &loc);
	axcheck(res, ax_log(res));
	io_str(loc);*/
	seq_loc loc = {0};
	res = seq_find(L"adhf\"a[sec]:jd\"ske\"j[sec]:\"j[sec]:sl", L"(!:`\"$\"`)[\x2<{a-z}+{[-]}>\x3]", &loc);
	ax_log(res);
	io_str(loc.beg);
	io_str(loc.end);

	/*const c16 *text = L"some_text,other_text";
	const c16 *loc = nullptr;
	res = find_substr_range(text, L"er", 12, 20, &loc);
	axcheck(res, ax_log(res));
	io_str(loc);*/

	io_i64(_MEM_ACTIVE);
	axcheck(res, ax_log(res));
	unref(res);

	return 0;
}

