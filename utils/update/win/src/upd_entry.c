#include "upd_parser.h"
#include "ax_control.h"

int main(){
	AX_DATA_ROOT root;
	ax_get_data_root(&root);

	AX_DATA_NODE* default_data = ax_get_data_node_d();
	for (uint32_t i = 0; i < AX_DATA_NODE_COUNT_D; i++){
		ax_set_data(&root, &default_data[i]);
	}

	ax_free_root(&root);

	ax_control_setup_i();
	ax_driver_setup_i();

	while(1);
	return 0;
}

