#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	u32 size = 0;
	noded_sect *sect = nullptr;

	res = noded_load_sect(L"C:\\update\\ax_info.noded", L"mysect", &sect);
	ax_log(res);
	//io_str(sect->name);

	return 0;
}

