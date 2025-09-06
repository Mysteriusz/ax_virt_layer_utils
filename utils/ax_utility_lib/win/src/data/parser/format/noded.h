#if !defined(AX_PARSER_NODED_INT)
#define AX_PARSER_NODED_INT

#include "ax_parser.h"

/*
 
   	.noded file syntax defnition. 

	==========================================

	[section_name]: -> Named section of stored nodes. 
	Entire section name CAN`T exceed 512 bytes.
	Section syntax is strict and HAS to always follow the same pattern.
	Section names CAN`T repeat.
	Sections always have to end with ':' character.
	They also CAN contain white spaces but CAN`T be multi-lined.

	Examples:
	[[

	[section a]:
	(node 1)
	(node 2)
	(node 3)

	[section_b]:
	(node 4)
	(node 5)

	]]

	[[
		
	[secti\non a]: -> INVALID (have to be one-lined)
	[section a] -> INVALID (no ':' character)

	]]

	==========================================

	value_name	=		1		;
	^		^		^		^
	|		|		|		|
	Key		Delimiter	Value		Suffix

	value_name=1; -> Key-value pair node. 
	Node name(key) CAN be contained in: (ANY of the following)
		[Key]
		<Key>

	Nodes must end with a suffix as: (ANY of the following)
		<Suffix>
		;
		L'\n' (new-line)

	Nodes must contain a delimiter as: (ANY of the following)
		=
		:=
		::

	As sections node CAN contain white spaces but CAN`T be multi-lined.

	Examples:
	[[

	[section a]:
	node 1=1;
	[node2] := 2;
	node 5=5<my_suffix>
	node 6 = 5.my_suffix.
	<node 7> = 5;

	]]

*/

#define NODED_EXT		L".noded"

typedef struct _noded_doc noded_doc;
typedef struct _noded_sect noded_sect;
typedef struct _noded_kvp noded_kvp;

typedef struct _noded_doc{
	io_file			*file;
	noded_sect		*root; 		
} noded_doc;
typedef struct _noded_sect{
	c16			*name;
	u32			line;
	noded_sect		*next;
} noded_sect;
// kvp - key-value-pair
typedef struct _node_kvp{
	c16			*key;
	u32			line;
	void			*val;
	noded_sect		*sect;
	struct node_kvp		*next;
} node_kvp;

/*
 	noded_doc related
*/

axres noded_load_doc(
	_in const c16		*path,
	_out noded_doc		**doc
);
axres noded_unload_doc(
	_in noded_doc		*doc
);
axres noded_inv_doc(
	_in noded_doc		*doc
);

/*
 	noded_sect related
*/

#define NODED_SECT_SET 		L"["
#define NODED_SECT_END 		L"]"
#define NODED_SECT_PTR 		L":"

#define NODED_SECT_BOUND 	0x200
axres noded_load_sect(
	_in const c16		*path,
	_in const c16		*sect_name, 
	_out noded_sect		**sect
);
axres noded_find_sect(
	_in io_file		*file,
	_in const c16		*sect_name, 
	_out u64		*offset
);

/*
 	noded_kvp related
*/

#endif // !defined(AX_PARSER_NODED_INT)

