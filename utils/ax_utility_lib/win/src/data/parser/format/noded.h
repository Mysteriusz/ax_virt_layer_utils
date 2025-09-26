#if !defined(AX_PARSER_NODED_INT)
#define AX_PARSER_NODED_INT

#include "ax_parser.h"

/*
 
   	.noded file syntax defnition. 

	==========================================

	[section_name]: -> Named section of stored nodes. 
	Section syntax is strict and HAS to always follow the same pattern.
	Section names CAN`T repeat.
	Sections always have to end with ':' character.
	Sections name is limited to <0x20-0x7f> (Printable ASCII).
	Sections CAN`T be multi-lined.

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

#define NODED_EXT		L"noded"

typedef struct _noded_doc noded_doc;
typedef struct _noded_sect noded_sect;
typedef struct _noded_kvp noded_kvp;

typedef struct _noded_doc{
	io_file			*file;
	ax_list			*sect_list; // List of noded_sect
} noded_doc;
bool noded_doc_inv(
	_in noded_doc 		*doc
);

typedef struct _noded_sect{
	c16			*label;
	u64			offset;
	ax_list 		*kvp;
} noded_sect;
// kvp - key-value-pair
typedef struct _node_kvp{
	c16			*key;
	u32			line;
	void			*val;
	noded_sect		*sect;
	noded_kvp		*next;
} node_kvp;

/*
 	noded global
*/
axres noded_load_sym(
	_in noded_doc 		*doc
);

/*
 	noded_doc related
*/

// Initialize document by loading symbols etc.
axres noded_init_doc(
	_in const c16		*path,
	_out noded_doc		**buf
);
axres noded_load_doc(
	_in noded_doc		*doc
);
axres noded_unload_doc(
	_in noded_doc		*doc
);

/*
 	noded_sect related
*/

#define NODED_SECT_BEG 		L"["
#define NODED_SECT_END 		L"]:\n"

// All ASCII writable without L'[' AND L']' characters
#define NODED_SECT_FMT		FMT_GRP L"[<" SEQ_CAP_ASCII L"-{[}-{]}>]:" FMT_GRP

// Load section and it`s nodes
axres noded_load_sect(
	_in noded_doc		*doc,
	_in const c16		*sect_name
);

/*
 	noded_kvp related
*/

#endif // !defined(AX_PARSER_NODED_INT)

