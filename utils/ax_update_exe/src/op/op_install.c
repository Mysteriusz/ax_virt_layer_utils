#include "op.h"
#include "noded.h"

/*
	Installation flow for 64bit Windows

	1) Obtain path to the driver (ax_virt_layer.sys)
*/

static const c16 *name = u"Install";

#define DEFAULT_INSTALL_URI URI_FILE u""

struct op_err op_install_call(
	struct op_meta 		*meta
){
	struct op_err res = {
		.op_name = name,
		.op_code = AX_SUCC
	};

	if (meta == nullptr){
		res.op_code = AX_INV_ARG;
		return res;
	}

	if (meta->cfg_uri == nullptr){
		open_data("file://", );
	}else{
		//open_data(meta->cfg_uri, );
	}

	return res;
}

