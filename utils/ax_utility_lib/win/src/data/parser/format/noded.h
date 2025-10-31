#if !defined(AX_PARSER_NODED_INT)
#define AX_PARSER_NODED_INT

#include "ax_parser.h"
#include "ax_parser_seq.h"

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
		[section a]:
		node 1 = a
		node 2 = b
		node 3 = c

		[section_b]:
		node 4 = d
		node 5 = e
		
	Invalid:
		[secti\non a]: -> INVALID (have to be one-lined)
		[section a] -> INVALID (no ':' character)
		[se]ction a] -> INVALID
		[sectio[n a] -> INVALID

	==========================================

	node_name	=		1		;
	^		^		^		^
	|		|		|		|
	Key		Delimiter	Value		Suffix

	Key_name=1; -> Key-value pair node. 
	Node name CAN be contained in: (ANY of the following)
		[Name]

	Nodes must end with a suffix as: (ANY of the following)
		;
		L'\n' (new-line)

	Nodes must contain a delimiter as: (ANY of the following)
		=
		:=
		::

	As sections node CAN contain white spaces but CAN`T be multi-lined.

	Examples:
		[section a]:
		node 1=1;
		[node2] := 2;
		node 3=5
		node 4 = 5
		[node 5] :: 5

*/

#define NODED_EXT		u"noded"

typedef struct _noded_doc noded_doc;
typedef struct _noded_sect noded_sect;
typedef struct _noded_kvp noded_kvp;

typedef struct _noded_doc{
	io_file			*file;
	ax_list			*sect_list; // List of noded_sect
} noded_doc;

typedef struct _noded_sect{
	c16			*name;
	const c16		*beg;
	const c16		*end;
	noded_doc 		*doc;
	ax_list			*kvp_list; // List of noded_kvp
} noded_sect;

typedef struct _noded_kvp{
	c16			*name;
	c16			*value;
	noded_sect 		*sect;
} noded_kvp;

/*
 	noded global
*/
bool noded_doc_inv(
	_in noded_doc 		*doc
);
bool noded_sect_inv(
	_in noded_sect 		*sect
);

/*
 	noded_doc related
*/

// Initialize document by loading symbols etc.
axres noded_doc_load(
	_in const c16		*path,
	_out noded_doc		**doc
);
// Call only after noded_load_doc to uninitialize all symbols
axres noded_doc_unload(
	_in noded_doc		*doc
);

/*
 	noded_sect related
*/

// Sequence format for noded_sect
#define NODED_SECT_FMT		u"(!:\".\")" /* Not in between " characters (string literal) */ \
				u"(!:\'.\')" /* Not in between " characters (string literal) */ \
				u"^[<" CAPTURE_FMT_ASCII u"-{[}-{]}" u">]:" /* Main section block sequence */ \
				u"<" CAPTURE_FMT_NL u">$" /* New line capture group */

// Load section and it`s nodes
axres noded_sect_load(
	_in noded_doc		*doc,
	_in seq_loc		sect_loc
);
axres noded_sect_unload(
	_in noded_sect		*sect
);

/*
 	noded_kvp related
*/

// Sequence format for noded_kvp
#define NODED_KVP_FMT		u"?[" CAPTURE_FMT_ASCII u"?]" /* Main name block sequence */ \
				u"<" CAPTURE_FMT_NL u">$" /* New line capture group */

// Load kvp
axres noded_kvp_load(
	_in noded_sect		*sect,
	_in const c16		*kvp_loc
);
axres noded_kvp_unload(
	_in noded_kvp		*kvp
);

#endif // !defined(AX_PARSER_NODED_INT)

