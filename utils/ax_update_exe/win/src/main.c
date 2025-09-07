#include "ax_utility.h"
#include "noded.h"

int main(
	void
){
	axres res = AX_SUCC;

	noded_sect *sect = nullptr;
	res = noded_load_sect(L"D:\\share\\ax_info.noded", L"config", &sect);
	axcheck(res);

	return 0;
}

