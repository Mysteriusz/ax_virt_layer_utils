#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	data_handle hdl = {0};
	c16 *ptr = L"my_value";
	open_data(
		L"reg://HKEY_LOCAL_MACHINE//Software//AX_VIRTUALIZATION", 
		URI_RULE_WRITE | URI_RULE_READ | URI_RULE_CREATE | URI_RULE_ADM,
		&hdl
	);

	/*u32 s = 0;
	c16* b = nullptr;

	push_data_reg(&hdl, REG_SZ, L"base_directory");

	res = hdl.ops->read(&hdl, &s, b);
	b = malloc(s);
	res = hdl.ops->read(&hdl, &s, b);

	pop_data_reg(&hdl);*/

	/*push_data_reg(&hdl, REG_SZ, L"base_directory");
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);
	pop_data_reg(&hdl);*/

	/*open_data(
		L"dir://C://", 
		URI_RULE_READ | URI_RULE_WRITE | URI_RULE_CREATE,
		&hdl
	);*/

	push_data_reg(&hdl, REG_SZ, L"base_directory");
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);

	u32 s = 0;
	c16 *buf = nullptr;

	res = hdl.ops->read(&hdl, &s, buf);
	buf = malloc(s);
	s--;
	res = hdl.ops->read(&hdl, &s, buf);

	ax_log(res);
	printf("%ls\n", buf);
	pop_data_reg(&hdl);

	close_data(&hdl);

	return 0;
}

