#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	data_handle hdl = {0};
	c16 *ptr = L"my_value";
	open_data(
		L"reg://HKEY_LOCAL_MACHINE//Software//AX_VIRTUALIZATION", 
		URI_RULE_WRITE | URI_RULE_CREATE | URI_RULE_ADM,
		&hdl
	);

	push_data_reg(&hdl, REG_SZ, L"base_directory");
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);
	pop_data_reg(&hdl);

	/*open_data(
		L"dir://C://", 
		URI_RULE_READ | URI_RULE_WRITE,
		&hdl
	);*/

	/*push_data_dir(&hdl, L".noded", L"base_directory");
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);
	printf("%ls\n", (c16*)hdl.con.user_data);
	pop_data_dir(&hdl);*/

	close_data(&hdl);

	ax_log(res);

	return 0;
}

