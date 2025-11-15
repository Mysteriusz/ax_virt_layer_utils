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
	Key		Delimiter	Value		Suffix (Not available)

	Key_name=1; -> Key-value pair node. 

	Nodes must end with a suffix as: (ANY of the following)
		u'\n' (new-line)

	Nodes must contain a delimiter as: (ANY of the following)
		=

	Same as sections node`s CAN contain white spaces but CAN`T be multi-lined.

	Examples:
		[section a]:
		node 1=1
		node2=some_value
		node 3=5
		node 4= 5
		node 5=some_value

*/

#define NODED_EXT		u"noded"

typedef struct _noded_doc noded_doc;
typedef struct _noded_sect noded_sect;
typedef struct _noded_kvp noded_kvp;

typedef struct _noded_doc{
	io_file			*file;
	ax_dict			*sect_dict; // Dictionary (name, _noded_sect)
} noded_doc;

typedef struct _noded_sect{
	c16			*name;
	ax_dict			*kvp_dict; // Dictionary (name, _noded_kvp)
} noded_sect;

typedef struct _noded_kvp{
	c16			*name;
	c16			*value;
} noded_kvp;

/*
	This structure allows for template for noded_kvp string representation.
	Serializer follows this flow:
		- key_lc -> kvp->name -> key_rc -> div -> val_lc -> key->value -> val_rc -> suff
*/
struct noded_kvp_temp{
	const c16 		*suff; // Suffix for the kvp line (string at the EOL)
	const c16 		*div; // key - value divider (string in between)
	const c16 		*key_lc; // Left key container string
	const c16 		*key_rc; // Right key container string
	const c16 		*val_lc; // Left value container string
	const c16 		*val_rc; // Right value container string
};

// Default KVP node template
static const struct noded_kvp_temp NODED_KVP_DEF = {
	.suff = u"",
	.div = u"=",
	.key_lc = u"", .key_rc = u"",
	.val_lc = u"", .val_rc = u"\n",
};

/*
 	noded global
*/
bool noded_doc_inv(
	_in noded_doc 		*doc
);
bool noded_sect_inv(
	_in noded_sect 		*sect
);
bool noded_kvp_inv(
	_in noded_kvp 		*kvp
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

// Save file into it`s io_file
axres noded_doc_save(
	_in noded_doc		*doc
);

/*
 	noded_sect related
*/

// Sequence format for noded_sect
#define NODED_SECT_FMT		u"(!:\".\")(!:\'.\')" /* Not in between " characters (string literal) */ \
				u"^\\[" \
				u"[n:sect_name;s:1]" \
				u"<" CAPTURE_FMT_ASCII u"-{[}-{]}" u">" /* Main section block sequence */ \
				u"]:" \
				u"<" CAPTURE_FMT_NL u">$" /* New line capture group */

// Load section and it`s nodes
axres noded_sect_load(
	_in noded_doc		*doc,
	_in seq_loc		*sect_loc
);
axres noded_sect_unload(
	_in noded_sect		*sect
);

// Convert section and it`s noded_kvp nodes to c16* string
axres noded_sect_c16(
	_in noded_sect			*sect,
	_in const struct noded_kvp_temp	*temp,
	_in_out u32			*size,
	_in_out _eval c16		*buf // Evaluate by using (size * sizeof(c16))
);

/*
 	noded_kvp related
*/

// Sequence format for noded_kvp
#define NODED_KVP_FMT		u"[n:kvp_name;s:1]" \
				u"<" CAPTURE_FMT_ASCII u">" \
				u"=" \
				u"[n:kvp_val;s:1]" \
				u"<" CAPTURE_FMT_ASCII u">"

// Load kvp
axres noded_kvp_load(
	_in noded_sect		*sect,
	_in const c16		*kvp_loc
);
axres noded_kvp_unload(
	_in noded_kvp		*kvp
);

// Convert section and it`s noded_kvp nodes to c16* string
axres noded_kvp_c16(
	_in noded_kvp			*kvp,
	_in const struct noded_kvp_temp	*temp,
	_in_out u32			*size,
	_in_out _eval c16		*buf
);

#endif // !defined(AX_PARSER_NODED_INT)

