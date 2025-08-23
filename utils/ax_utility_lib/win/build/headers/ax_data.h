#if !defined(AX_DATA_INT)
#define AX_DATA_INT

#include "ax_parser.h"

#include <stdlib.h>

typedef struct _data_handle data_handle; 

/*

	Context structure id`s and defaults 

*/

#define CON_REG 		0x01
#define CON_DIR 		0x02
#define CON_FILE 		0x03
typedef struct _data_con{
	u8 			id; // Context identifier
	void 			*data; // Context user-defined data
} data_con;

/*

	 Operator function declarations

*/

typedef axres (*data_read)(
	_in data_handle		hdl,	
	_out u32		size,
	_in_out void		*buf
);
struct data_ops{
	data_read 		read;
};

/*

	 Data handle structure

*/

typedef struct _data_handle{
	data_con 		con; // Context structure
	const struct data_ops 	*ops; // Operator table
} data_handle;

#define URI_REG 		L"reg://"
#define URI_DIR 		L"dir://"
#define URI_FILE 		L"file://"

axres open_data(
	_in c16			*uri,
	_out data_handle	*hdl
);
axres close_data(
	_in data_handle 	*hdl
);

/*
 	
   	Registry interface

*/

#if !defined(AX_DATA_REG_INT) && defined(AX_WIN32)
#define AX_DATA_REG_INT

axres ops_read_reg(
	_in data_handle		hdl,	
	_out u32		size,
	_in_out void		*buf
);

// Default registry data_ops structure
static const struct data_ops _ops_reg = { 
	.read = ops_read_reg 
};

axres open_data_reg(
	_in c16			*uri,
	_out data_handle	*hdl
);
axres close_data_reg(
	_in data_handle 	*hdl
);

#endif // !defined(AX_DATA_REG_INT)

#endif // !defined(AX_DATA_INT)


