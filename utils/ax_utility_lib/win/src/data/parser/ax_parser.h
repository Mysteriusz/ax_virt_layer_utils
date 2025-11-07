#if !defined(AX_PARSER_INT)
#define AX_PARSER_INT 

#include "ax_type.h"
#include "ax_error.h"
#include "ax_io.h"

// UTF-16 string length in characters
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
// UTF-16 string length in bytes
static inline u32 _c16len_b(
	_in const c16 		*text
){
	return _c16len(text) * sizeof(c16);
}

static inline c16 *_c16dup(
	_in const c16 		*text
){
	u32 text_size = _c16len_b(text);
	c16 *ptr = axmalloc(text_size + sizeof(c16));
	memcpy(ptr, text, text_size);
	ptr[text_size - 1] = L'\0';
	return ptr;
}
// Checks for L'\\' before text_char
static inline bool _is_esc(
	_in const c16 		*text,
	_in const c16 		*text_char
){
	const c16 *esc_char = (text_char - 1);
	if (esc_char >= text
	&& *esc_char == L'\\'){
		return true;
	}

	return false;
}
// Checks for L'?' before text_char
static inline bool _is_opt(
	_in const c16 		*text,
	_in const c16 		*text_char
){
	const c16 *opt_char = (text_char - 1);
	if (opt_char >= text
	&& *opt_char == L'?'){
		return true;
	}

	return false;
}

#define end_c16(tp)		((c16*)(tp + _c16len(tp)))
#define dif_c16(s,e)		((c16*)(e) - (c16*)(s))
#define in_c16(tp,p)		(dif_c16(tp,p) < _c16len(tp))
#define in_c16_s(tp,p,s)	(dif_c16(tp,p) < (s))

#define is_crlf(tp)		(*(tp) == L'\r' && *((tp) + 1) == L'\n')
#define is_lf(tp)		(*(tp) == L'\n')

#define dif_b(s,e)		((u8*)(e) - (u8*)(s))

#define TO_STR1(chr)		#chr
#define TO_STR(chr)		TO_STR1(chr)	

// Join 2 typeof(c16) characters
#define JOIN_C16_C16(chr1,chr2)	((c16[3]){chr1, chr2, u'\x0'})

/*
 
	Write-back of any function occurs ONLY when:
		- return of type axres == AX_SUCC
		- return of any pointer != nullptr

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
	_in u32 		a,
	_in u32 		b,
	_in const c16		value	
);
axres compare(
	_in const c16 		*a,
	_in const c16		*b	
);
axres charset_count(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u32		*count
);
axres line_count(
	_in const c16 		*text,
	_out u32		*count
);
axres reverse(
	_in const c16 		*text,
	_in_out c16 		*rev
);
axres c16_cat(
	_in const c16 		*a,
	_in const c16 		*b,
	_in_out u32		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres c16_union(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u32 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres c16_difference(
	_in const c16 		*a,
	_in const c16		*b,
	_in_out u32 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
// Remove all occurences of b in string a
axres c16_remove(
	_in const c16 		*a,
	_in const c16		b,
	_in_out u32 		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres trim(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u32		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

/*
 
 	Skip interface

*/

#if !defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_SKIP_INT

#define UTF16_EOT		u'\u0003'
#define UTF16_EOT_STR		u"\u0003"

#define UTF16_ANY		u'\uFFFD'
#define UTF16_ANY_STR		u"\uFFFD"

#define CHARSET_NL 		u"\x0a\x0d"
#define CHARSET_WS 		u"\x20\t"
#define CHARSET_PUNCT     	u".,;:!?()[]{}"

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
axres find_substr_range(
	_in const c16 		*text,
	_in const c16 		*substr,
	_in u32			from,
	_in u32			to,
	_out const c16		**loc // text substr start location
);

axres find_parentheses(
	_in const c16 		*text,
	_in c16			parens,
	_out_opt const c16	**loc // text location of the opposite parentheses character
);

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
	_out u32		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres read_until(
	_in const c16 		*text,
	_in const c16		*charset,
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

// Read (inclusive from) range 
axres read_range(
	_in const c16 		*text,
	_in u32			from,
	_in u32			to,	
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

axres read_line(
	_in const c16 		*text,
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres read_word(
	_in const c16 		*text,
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

axres join_with(
	_in _eval c16		*buf, // Evaluate by using (size * sizeof(c16))
	_in_out u32		*size,
	_in u32 		n,
	...
);

axres split_by(
	_in const c16		*text,
	_in const c16		*charset,
	_out u32		*size, 
	_in_out _eval c16	**buf // Evaluate by using (size * sizeof(c16*))
);
axres split_by_c(
	_in c16			**buf,
	_in u32			size
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
	_in_out u32		*size, // _in for buffer size safety
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

#endif // !defined(AX_PARSER_FILE_INT)

#if defined(AX_PARSER_READ_INT) \
	&& defined(AX_PARSER_CONV_INT) \
	&& defined(AX_PARSER_FIND_INT) \
	&& defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_COMPLETE
#endif

#endif // !defined(AX_PARSER_INT)

