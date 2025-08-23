#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	data_handle h = {0};
	open_data(L"reg://HKEY_LOCAL_MACHINE", &h);

	ax_log(res);
	printf("%u", h.con.id);

	return 0;
}

