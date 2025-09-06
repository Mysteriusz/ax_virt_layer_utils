#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	//u32 size = 0;
	//noded_sect *sect = nullptr;

	//res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_RW, &buf);
	u64 off = 0;
	io_file file = {0};
	io_fo(L"D:\\share\\ax_info.noded", IO_FILE_R, &file);
	
	res = find_substr_f(&file, L"[mysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysectmysect]:", &off);
	if (AX_ERR(res)){
		ax_log(res);
		return 0;
	}

	file.offset = off;
	c16 *buf = axmalloc(50);
	io_fr(&file, 50, buf, nullptr);
	io_str(buf);

	//res = noded_load_sect(L"D:\\share\\ax_info.noded", L"mysect", &sect);
	//ax_log(res);
	//io_str(sect->name);

	return 0;
}

