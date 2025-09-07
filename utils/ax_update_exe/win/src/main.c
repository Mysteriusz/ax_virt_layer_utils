#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	//u32 size = 0;
	noded_sect *sect = nullptr;

	//res = io_fo(L"D:\\share\\ax_info.noded", IO_FILE_RW, &buf);

	res = noded_load_sect(L"D:\\sdhare\\ax_info.noded", L"mysect1", &sect);
	if (AX_ERR(res)){
		ax_log(res);
		return 0;
	}
	return 0;
}

