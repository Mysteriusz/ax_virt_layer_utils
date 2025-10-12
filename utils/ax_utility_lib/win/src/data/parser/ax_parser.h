#if !defined(AX_PARSER_INT)
#define AX_PARSER_INT 

#include "ax_type.h"
#include "ax_error.h"
#include "ax_io.h"

static inline u32 _c16len(
	_in const c16 		*text
){
	if (text == nullptr){
		return 0;
	}

	u32 len = 0;
	while(*text != L'\0'){
		len++;
		text++;
	}

	return len;
}
static inline u32 _c16len_b(
	_in const c16 		*text
){
	return _c16len(text) * sizeof(c16);
}

static inline c16 *_c16dup(
	_in const c16 		*text
){
	u64 text_size = _c16len_b(text);
	c16 *ptr = axmalloc(text_size + sizeof(c16));
	memcpy(ptr, text, text_size);
	ptr[text_size - 1] = L'\0';
	return ptr;
}

#define end_c16(tp)		((c16*)(tp + _c16len(tp)))
#define dif_c16(s,e)		((c16*)(e) - (c16*)(s))
#define in_c16(tp,p)		(dif_c16(tp,p) < _c16len(tp))
#define in_c16_s(tp,p,s)	(dif_c16(tp,p) < (s))

#define dif_b(s,e)		((u8*)(e) - (u8*)(s))

/*
 
   	If AX_NOT_FND is returned, every _out parameter
	will be written to ONLY when there was no error.
	(c16* -> *0)
	(c16** -> nullptr)

*/

/*
 
   	For returning buffer size the following notation should be used.

	// All arguments that cannot be nullptr.
	if (arg1 == nullptr
	|| arg2 == nullptr){
		return AX_INV_ARG;
	}

	bool ret_size = ((size != nullptr) && (buf == nullptr));
	if (!ret_size){
		// All arguments that can be nullptr.
		if (arg1 == nullptr
		|| arg2 == nullptr){
			return AX_INV_ARG;
		}
		if (size == nullptr
		|| buf == nullptr){
			return AX_INV_BUF;
		}
	}

*/

/*
 
 	Internal interface

*/

axres starts_with(
	_in const c16		*text,
	_in const c16		*str,
	_out_opt const c16	**loc
);
axres contains(
	_in const c16 		*text,
	_in const c16		value	
);
// Range containment check
axres contains_r(
	_in const c16 		*text,
	_in u64 		a,
	_in u64 		b,
	_in const c16		value	
);
axres compare(
	_in const c16 		*a,
	_in const c16		*b	
);
axres charset_count(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*count
);
axres reverse(
	_in const c16 		*text,
	_in_out c16 		*rev
);
axres c16_cat(
	_in const c16 		*a,
	_in const c16 		*b,
	_in_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres c16_union(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u64 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres c16_difference(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u64 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres trim(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

/*
 
 	Skip interface

*/

#if !defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_SKIP_INT

#define UNICODE_ANY		L"\uFEFE"
#define CHARSET_ANY 		UNICODE_ANY

#define CHARSET_NL 		L"\x0a\x0d"
#define CHARSET_WS 		L"\x20\t"
#define CHARSET_PUNCT     	L".,;:!?()[]{}"

axres skip_until(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
);
axres skip_until_n(
	_in const c16		*text,
	_in const c16		*charset,
	_in u32			n,
	_out const c16		**loc
);
axres skip_until_r(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
);

axres skip_word(
	_in const c16 		*text,
	_in c16			*word,
	_out const c16		**loc
);

axres skip_while(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
);

axres skip_line(
	_in const c16		*text,
	_out const c16		**loc
);
axres skip_line_n(
	_in const c16		*text,
	_in u32			n,
	_out const c16		**loc
);

axres skip_ws(
	_in const c16		*text,
	_out const c16		**loc
);

#endif // !defined(AX_PARSER_SKIP_INT)

/*
 
 	Find interface

*/

#if !defined(AX_PARSER_FIND_INT) && defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_FIND_INT

axres find_char(
	_in const c16 		*text,
	_in const c16 		val	
);

axres find_substr(
	_in const c16 		*text,
	_in const c16 		*substr,
	_out const c16		**loc, // text substr start location
	_out_opt const c16	**sub_loc  // substr partially found offset
);

#if !defined(AX_PARSER_SEQUENCE_INT)
#define AX_PARSER_SEQUENCE_INT

// Charset of sequence starting identifiers
#define CHARSET_SEQ 			L"<\x2\x3\\"
typedef struct _fmt_group{
	ax_list *spec_list; // List of _fmt_spec
	ax_list *cond_list; // List of _fmt_cond
} fmt_group;

enum spec_type{
	sequence = 0,
	capture_set = 1,
	control_beg = 2,
	control_end = 3,
};
typedef struct _fmt_spec{
	const c16 *value;
	enum spec_type type;
} fmt_spec;

enum cond_mode{
	condition_bef = 0x1,
	condition_aft = 0x2
};
typedef struct _fmt_cond{
	bool ret;
	enum cond_mode mode;
	const c16 *bef; // Before $ string
	const c16 *aft; // After $ string
} fmt_cond;

typedef struct _seq_loc{
	const c16 *beg;
	const c16 *end;
} seq_loc;

// Format group character 
#define FMT_GRP 		L"\\"
#define CAPTURE_FMT_NL		L"{\r}+{\n}"

// LIFO sequence finder stack  
axres seq_split_fmt(
	_in const c16 		*fmt,
	_out ax_list 		**grp_list // Access by index_as(locs, 0, fmt_group*)
);
axres seq_match(
	_in const c16		*text,
	_in const c16 		*seq_set,
	_in const c16 		*seq_end,
	_in const c16		*cap
);
axres seq_locate(
	_in const c16		*text,
	_in fmt_group 		*grp,
	_out seq_loc		*loc
);

/*
 	Known issues:
		- seq: L"\\[<{a-z}>|<{.}>]:\\"
		- text: L"l[section|other_text||a|b]:"
		Result is AX_NOT_FND.
		Cause and possible fix:
			seq_locate searches for next sequences in order so if next sequence
			is the same as the character inside of the current section the it assumes
			that || means 2 sections itself so 
			->  seq_list[1] = | AND seq_list[2] = | which is not true
*/
axres seq_find(
	_in const c16		*text,
	_in const c16 		*fmt,
	_out seq_loc		*loc
);
axres seq_find_all(
	_in const c16		*text,
	_in const c16 		*fmt,
	_in_out ax_list		*locs // Access by index_as(locs, 0, seq_loc*)
);

#define CAPTURE_FMT_ASCII	L"{\x20-\x7f}"

/*
 	SEQUENCE INTERNAL UTILITIES
*/

// ====================== CHARSET ======================

// Syntax invalidation for seq_cap_to_charset
bool seq_cap_to_charset_inv(
	_in const c16		*cap // cap FOR capture group
);
/*
   	Input of cap has to start with:
		L'{'
	and end with:
		L'}'

	Example:
		{.}
		{a-z}+{x-z}+{_}
		{l-n}+{x}
		{\x20-\x45}+{\x6f}

	In case you want to include syntax characters ({, }, -)
	You either have to include them in the capture range like above
	OR you can add them by adding single character like so:
		{a-z}+{{}+{-}+{<}
*/
_free const c16 *seq_cap_to_charset(
	_in const c16		*cap // cap FOR capture group
);

/*
 	Capture set parsing.

	Input:
		L"<{a-k}-{d}>"
	Output:
		Parsed capture set in form of a string L"abcefghijk"
*/
axres seq_group_capture_set(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*spec_list,
	_out const c16		**loc
);

// ====================== FUNCTION ======================

#define SEQ_COND_CHARSET L"$"

/* 
	Function has to consist of at least one control character.
	Recognized characters:
		$ - Any character
*/
bool seq_func_to_cond_inv(
	_in const c16		*func // func for function
);
/*
   	Input of func has to start with:
		L'('
	and end with:
		L')'

	Example:
		(+:[[$]])
		(!:<$>)
		(!::$>:)
	
	Returns HEAP ALLOCATED condition function
*/
_free fmt_cond *seq_func_to_cond(
	_in const c16		*func // func for function
);

/*
 	Condition parsing.

	Input:
		L"(!:[$])"
	Output:
		Parsed function that validates if ANY next char is NOT between L'[' AND L']'
*/
axres seq_group_condition(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*spec_list,
	_out const c16		**loc
);

#endif // !defined(AX_PARSER_SEQUENCE_INT)

#endif // !defined(AX_PARSER_FIND_INT)

/*
 
 	Read interface

*/

#if !defined(AX_PARSER_READ_INT) && defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_READ_INT

// TODO:
// Read from end until character in the charset
axres read_until_r(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres read_until(
	_in const c16 		*text,
	_in const c16		*charset,
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

// Read (inclusive from) range 
axres read_range(
	_in const c16 		*text,
	_in u64			from,
	_in u64			to,	
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

axres read_line(
	_in const c16 		*text,
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres read_word(
	_in const c16 		*text,
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

axres join_with(
	_in _eval c16		*buf, // Evaluate by using (size * sizeof(c16))
	_in_out u64		*size,
	_in u32 		n,
	...
);

axres split_by(
	_in const c16		*text,
	_in const c16		*charset,
	_out u64		*size, 
	_in_out _eval c16	**buf // Evaluate by using (size * sizeof(c16*))
);
axres split_by_c(
	_in c16			**buf,
	_in u64			size
);

#endif // !defined(AX_PARSER_READ_INT)

/*
 
 	Convertion interface

*/

#if !defined(AX_PARSER_CONV_INT)
#define AX_PARSER_CONV_INT 

axres c16_to_int(
	_in const c16		*text,
	_out iu64 		*buf	
);

#endif // !defined(AX_PARSER_CONV_INT)

#if !defined(AX_PARSER_FILE_INT) && defined(AX_PARSER_FIND_INT) && defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_FILE_INT

// Can optimize after adding skip_until_r
axres check_ext(
	_in const c16 		*path,
	_in const c16		*ext // Extension WITHOUT '.'
);

/*
	Find interface for files
*/
axres find_substr_f(
	_in io_file		*file,
	_in const c16 		*substr
);

/*
	Seq interface for files
*/
axres seq_find_f(
	_in io_file		*file,
	_in const c16 		*fmt
);
axres seq_find_all_f(
	_in io_file		*file,
	_in const c16 		*fmt,
	_in_out ax_list		*locs // Access by index_as(locs, 0, seq_loc*)
);

/*
	Skip interface for files
*/
axres skip_while_f(
	_in io_file		*file,
	_in const c16		*charset
);
axres skip_until_f(
	_in io_file		*file,
	_in const c16		*charset
);
axres skip_line_f(
	_in io_file		*file
);
axres skip_word_f(
	_in io_file		*file
);

axres read_line_f(
	_in io_file		*file,
	_in_out u64		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

#endif // !defined(AX_PARSER_FILE_INT)

#endif // !defined(AX_PARSER_INT)

