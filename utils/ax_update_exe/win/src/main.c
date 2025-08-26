#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	data_handle hdl = {0};
	open_data(
		L"reg://HKEY_LOCAL_MACHINE//Software//AX_VIRTUALIZATION", 
		URI_RULE_WRITE | URI_RULE_ADM,
		&hdl
	);

	c16 *ptr = L"my_value";
	/*hdl.con.user_data = (void*)L"some_key";
	hdl.ops->read(&hdl, &s, nullptr);
	ptr = malloc(s);
	hdl.ops->read(&hdl, &s, (void*)ptr);*/

	PUSH_DATA_REG(&hdl, REG_SZ, L"base_directory");
	res = hdl.ops->write(&hdl, sizeof(L"my_value"), ptr);
	POP_DATA_REG(&hdl);

	close_data(&hdl);

	ax_log(res);
	printf("%ls\n", ptr);

	return 0;
}

