#include "ax_utility.h"

int main(
	void
){
	axres res = AX_SUCC;

	/*u32 s = 0;
	c16** ptr = nullptr;
	res = split_by(L"fsdfsdff\\d\\somevar", L"\\", &s, ptr);
	ptr = malloc(s * sizeof(c16*));
	res = split_by(L"fsdfsdff\\d\\somevar", L"\\", &s, ptr);
	c_split_by(ptr, s);*/

	data_handle hdl = {0};
	open_data(L"reg://HKEY_LOCAL_MACHINE//Software//AX_VIRTUALIZATION//next//hive", URI_RULE_ADM | URI_RULE_CRT, &hdl);
	hdl.con.user_data = _wcsdup(L"some_key");

	u32 s = 0;
	hdl.ops->read(&hdl, &s, nullptr);
	c16 *ptr = malloc(s);
	hdl.ops->read(&hdl, &s, (void*)ptr);

	ax_log(s);
	printf("%ls\n", ptr);

	return 0;
}

