#include "D:/ax_project/ax_virt_layer_utils/utils/base/src/ax_utility.h"
#include "stdio.h"

int main(){
   	int arg_count = 0;
    	LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &arg_count);

	AX_DATA_ROOT root;
	ax_get_data_root(&root);

	ax_set_data(&root, &AX_DATA_NODE_BSD, "test", sizeof("test"));

	AX_DATA_NODE node = AX_DATA_NODE_BSD;
	ax_get_data(&root, &node);
	printf(node.value);

	while(1);	
    	return 0;
}
