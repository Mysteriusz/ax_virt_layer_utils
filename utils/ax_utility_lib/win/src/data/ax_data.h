#if !defined(AX_DATA_INT)
#define AX_DATA_INT

#include "ax_parser.h"

#include <stdlib.h>

typedef struct _data_handle data_handle; 

/*

	Context structure id`s and defaults 

*/

#if defined(AX_WIN32)

#define CON_REG 		0x01
#define CON_REG_SIZE 		sizeof(HKEY)

#endif

#define CON_DIR 		0x02
#define CON_FILE 		0x03

typedef struct _data_con{
	u8 			id; // Context identifier
	c16			*path; // Path of the initial URI
	i64			rule; // Rules of the handle
	bool 			is_open; // Is handle active
	/*
		FOR id == CON_REG
			data = HKEY
		FOR id == CON_DIR
		FOR id == CON_FILE
	*/
	void			*data; // System defined context data
	void			*user_data; // User defined context data
} data_con;

/*

	 Operator function declarations

*/

typedef axres (*read_data)(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
);
struct data_ops{
	read_data 		read;
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

#define URI_RULE_ADM		0x0001 // Request full access (KEY_ALL_ACCESS for the (HKEY)hdl->con.data)
#define URI_RULE_CRT		0x0002 // Enforce creation of the uri (if doesn`t already exist)

axres open_data(
	_in c16			*uri,
	_in i64			rule,
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

axres read_data_reg(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
);

// Default registry data_ops structure
static const struct data_ops _ops_reg = { 
	.read = read_data_reg 
};

axres open_data_reg(
	_in c16			*uri,
	_in i64			rule,
	_out data_handle	*hdl
);
axres close_data_reg(
	_in data_handle 	*hdl
);

#endif // !defined(AX_DATA_REG_INT)

#endif // !defined(AX_DATA_INT)


