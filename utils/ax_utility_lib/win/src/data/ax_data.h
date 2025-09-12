#if !defined(AX_DATA_INT)
#define AX_DATA_INT

#include "ax_parser.h"

#if defined(AX_UM)
#include <stdio.h>
#endif

typedef struct _data_handle data_handle; 
typedef struct _data_con data_con; 

typedef axres (*read_data)(
	_in data_handle		*hdl,	
	_in_out u32		*size,
	_in_out _eval void	*buf // Evaluate by malloc(size)
);
/*
 	When writing REMEMBER to that string should not be null-terminated unless it`s an expected file ending
*/
typedef axres (*write_data)(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

// Invalidate common checks
axres read_data_inv(
	_in data_handle		*hdl,	
	_in u32			*size,
	_in void		*buf,
	_out bool		*ret // Indicator of early size only return
);
axres write_data_inv(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

/*

	Context structure id`s and defaults 

*/

#if defined(AX_WIN64)

#define CON_REG 		0x01
#define CON_REG_SIZE 		sizeof(HKEY)

#endif

#define CON_DIR 		0x02
#define CON_FILE 		0x03

typedef struct _data_con{
	u8 			id; // Context identifier
	c16			*path; // Duplicated path of the initial URI
	i64			rule; // Rules of the handle
	bool 			is_open; // Is handle active
	/*
		FOR id == CON_REG
			data = HANDLE as HKEY
		FOR id == CON_DIR
			data = nullptr 
		FOR id == CON_FILE
			data = io_file* 
	*/
	void			*data; // System defined context data
	void			*user_data; // User defined context data
} data_con;

/*

	 Operator function declarations

*/

/*
 	Every data_ops function must be validated by its 
	invalidation function before executing main logic.

	Example:
		def read_data(args)
			if err([read_data]_inv(args))
				ret

			**main logic**
			
*/
struct data_ops{
	read_data 		read;
	write_data 		write;
};

/*

	 Data handle structure

*/

typedef struct _data_handle{
	data_con 		con; // Context structure
	const struct data_ops 	*ops; // Operator table
} data_handle;
// Data handle invalidation check 
// TRUE if INVALID 
// FALSE if VALID 
bool data_handle_inv(
	_in data_handle		*hdl
);

#define URI_REG 		L"reg://"
#define URI_DIR 		L"dir://"
#define URI_FILE 		L"file://"

#define URI_RULE_ADM		0x0001 // Request full access (KEY_ALL_ACCESS for the (HKEY)hdl->con.data)

#define URI_RULE_CREATE		0x0002 // Allow for path/entry creation
#define URI_RULE_READ		0x0004 // Allow for reading 
#define URI_RULE_WRITE		0x0008 // Allow for writing

inline io_file_acc rule_to_io(
	_in i64			rule
){
	io_file_acc ret = 0;
	if (chkf(rule, URI_RULE_READ)){
		ret |= IO_FILE_R;
	}
	if (chkf(rule, URI_RULE_WRITE)){
		ret |= IO_FILE_W;
	}
	if (chkf(rule, URI_RULE_CREATE)){
		ret |= IO_FILE_C;
	}
	return ret;
}

// URI scheme validation
#define URI_V(uri_md, uri, uri_p)( \
	(starts_with(uri, uri_md, nullptr) != AX_SUCC) \
	|| (skip_word(uri, uri_md, &uri_p) != AX_SUCC) \
)

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

#if !defined(AX_DATA_REG_INT) && defined(AX_WIN64)
#define AX_DATA_REG_INT

typedef struct _data_reg_desc{
	u32		type; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
	c16		*name; // Pointer to the name buffer
} data_reg_desc;

axres push_data_reg(
	_in data_handle 	*hdl,
	_in u32			type,
	_in const c16		*name
);
axres pop_data_reg(
	_in data_handle 	*hdl
);

/*

 	Operations table interface intances

*/
// ops.read
axres read_data_reg(
	_in data_handle		*hdl,	
	_in_out u32		*size,
	_in_out _eval void	*buf
);
// ops.write
axres write_data_reg(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);


// Default registry data_ops structure
static const struct data_ops _ops_reg = { 
	.read = read_data_reg,
	.write = write_data_reg 
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

#if !defined(AX_DATA_DIR_INT)
#define AX_DATA_DIR_INT 

axres push_data_dir(
	_in data_handle 	*hdl,
	_in const c16		*ext,
	_in const c16		*name
);
axres pop_data_dir(
	_in data_handle 	*hdl
);

/*

 	Operations table interface intances

*/
// ops.read
axres read_data_dir(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out _eval void	*buf
);
// ops.write
axres write_data_dir(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

// Default directory data_ops structure
static const struct data_ops _ops_dir = { 
	.read = read_data_dir,
	.write = write_data_dir 
};

axres open_data_dir(
	_in c16			*uri,
	_in i64			rule,
	_out data_handle	*hdl
);
axres close_data_dir(
	_in data_handle 	*hdl
);

#endif // !defined(AX_DATA_DIR_INT)

#if !defined(AX_DATA_FILE_INT)
#define AX_DATA_FILE_INT

// Defines the syntax in which value is stored.
// TODO: XML,CSV,YAML and more formats support.
enum data_form{
	DF_NODED = 0,
};

axres push_data_file(
	_in data_handle 	*hdl,
	_in const c16		*label,
	_in const c16		*delim
);
axres pop_data_file(
	_in data_handle 	*hdl
);
 
/*

 	Operations table interface intances

*/
// ops.read
axres read_data_file(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out _eval void	*buf
);
// ops.write
axres write_data_file(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

// Default file data_ops table 
static const struct data_ops _ops_file = { 
	.read = read_data_file,
	.write = write_data_file 
};

axres open_data_file(
	_in c16			*uri,
	_in i64			rule,
	_out data_handle	*hdl
);
axres close_data_file(
	_in data_handle 	*hdl
);

#endif // !defined(AX_DATA_FILE_INT)

#endif // !defined(AX_DATA_INT)


