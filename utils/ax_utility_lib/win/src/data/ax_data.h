#if !defined(AX_DATA_INT)
#define AX_DATA_INT

#include "ax_parser.h"

#include <stdlib.h>

typedef struct _data_handle data_handle; 
typedef struct _data_con data_con; 

typedef axres (*read_data)(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
);
typedef axres (*write_data)(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

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
	c16			*path; // Duplicated path of the initial URI
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
// Data handle validation helper
// TRUE if INVALID 
// FALSE if VALID 
#define DATA_HANDLE_V(hdl) ({ \
	data_con con = (hdl)->con; \
	const struct data_ops *ops = (hdl)->ops; \
	bool con_data_v = (con.id == CON_DIR) \
		? (con.data == nullptr) \
		: (con.data != nullptr); \
	bool con_v = ( \
		(con.path != nullptr) \
		&& (con.is_open == true) \
		&& con_data_v \
	); \
	bool ops_v = ( \
		(ops != nullptr) \
		&& (ops->read != nullptr) \
		&& (ops->write != nullptr) \
	); \
	bool v = !(con_v && ops_v); \
	v; \
})

#define URI_REG 		L"reg://"
#define URI_DIR 		L"dir://"
#define URI_FILE 		L"file://"

#define URI_RULE_ADM		0x0001 // Request full access (KEY_ALL_ACCESS for the (HKEY)hdl->con.data)

#define URI_RULE_CREATE		0x0002 // Allow for path/entry creation
#define URI_RULE_READ		0x0004 // Allow for reading 
#define URI_RULE_WRITE		0x0008 // Allow for writing

// URI scheme validation
#define URI_V(uri_md, uri, uri_p)( \
	(starts_with(uri, uri_md) != AX_SUCC) \
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

#if !defined(AX_DATA_REG_INT) && defined(AX_WIN32)
#define AX_DATA_REG_INT

/*

 	Operations table interface intances

*/
// ops.read
axres read_data_reg(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
);
// ops.write
axres write_data_reg(
	_in data_handle		*hdl,	
	_in u32			size,
	_in void		*buf
);

typedef struct _data_reg_desc{
	DWORD		dwType; // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
	LPCWSTR		lpValueName; // Pointer to the name buffer
} data_reg_desc;

// Auto push data to the handle user_data variable.
#define	PUSH_DATA_REG(hdl, type, name)({ \
	data_reg_desc *desc = malloc(sizeof(data_reg_desc)); \
	desc->dwType = (type); \
	desc->lpValueName = name; \
	((hdl)->con).user_data = desc; \
})
// Auto clears data from the handle user_data variable.
#define POP_DATA_REG(hdl)({ \
	if (((hdl)->con).user_data != nullptr){ \
		free(((hdl)->con).user_data); \
	} \
})

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

/*

 	Operations table interface intances

*/
// ops.read
axres read_data_dir(
	_in data_handle		*hdl,	
	_out u32		*size,
	_in_out void		*buf
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

// Auto push data to the handle user_data variable.
#define	PUSH_DATA_DIR(hdl, ext, name)({ \
	u32 path_len = _c16len(((hdl)->con).path) * sizeof(c16); \
	u32 name_len = _c16len((name)) * sizeof(c16); \
	u32 ext_len = _c16len((ext)) * sizeof(c16); \
\
	c16 *fpath = malloc(path_len + name_len + ext_len + sizeof(c16)); \
\
	memcpy(fpath, ((hdl)->con).path, path_len); \
	memcpy(&fpath[path_len / sizeof(c16)], (name), name_len); \
	memcpy(&fpath[(path_len + name_len) / sizeof(c16)], (ext), ext_len); \
	((hdl)->con).user_data = fpath; \
})
// Auto clears data from the handle user_data variable.
#define POP_DATA_DIR(hdl)({ \
	if (((hdl)->con).user_data != nullptr){ \
		free(((hdl)->con).user_data); \
	} \
})

axres open_data_dir(
	_in c16			*uri,
	_in i64			rule,
	_out data_handle	*hdl
);
axres close_data_dir(
	_in data_handle 	*hdl
);

#endif // !defined(AX_DATA_DIR_INT)

#endif // !defined(AX_DATA_INT)


