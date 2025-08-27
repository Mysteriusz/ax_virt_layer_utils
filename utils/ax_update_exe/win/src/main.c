#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	data_handle hdl = {0};
	c16 *ptr = L"my_value";
	/*open_data(
		L"reg://HKEY_LOCAL_MACHINE//Software//AX_VIRTUALIZATION", 
		URI_RULE_WRITE | URI_RULE_CREATE | URI_RULE_ADM,
		&hdl
	);*/
	open_data(
		L"dir://C://", 
		URI_RULE_READ | URI_RULE_WRITE,
		&hdl
	);

	/*hdl.con.user_data = (void*)L"some_key";
	hdl.ops->read(&hdl, &s, nullptr);
	ptr = malloc(s);
	hdl.ops->read(&hdl, &s, (void*)ptr);*/

	PUSH_DATA_DIR(&hdl, L".noded", L"base_directory");
	printf("%ls\n", (c16*)hdl.con.user_data);
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);
	POP_DATA_DIR(&hdl);

	close_data(&hdl);

	ax_log(res);
	printf("%ls\n", ptr);

	return 0;
}

