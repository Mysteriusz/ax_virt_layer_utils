#include "D:/ax_project/ax_virt_layer_utils/utils/base/src/ax_utility.h"
#include "stdio.h"

int main(){
   	int arg_count = 0;
    	LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &arg_count);

	printf("%i", ax_get_data_root(NULL));

	while(1);	
    	return 0;
}
