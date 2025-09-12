#if !defined(AX_PARSER_INT)
#define AX_PARSER_INT 

#include "ax_type.h"
#include "ax_error.h"

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

#define end_c16(tp)		((c16*)(tp + _c16len(tp)))
#define dif_c16(s,e)		((c16*)e - (c16*)s)
#define in_c16(tp,p)		(dif_c16(tp,p) < _c16len(tp))
#define in_c16_s(tp,p,s)	(dif_c16(tp,p) < s)

#define dif_b(s,e)		((u8*)e - (u8*)s)

/*
 
   	If AX_NOT_FND is returned, every _out parameter
	will be set to it`s default type value.
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
axres compare(
	_in const c16 		*a,
	_in const c16		*b	
);
// TODO: FIX TRIM
axres trim(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);

/*
 
 	Find interface

*/

#if !defined(AX_PARSER_FIND_INT)
#define AX_PARSER_FIND_INt

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

#endif // !defined(AX_PARSER_FIND_INT)

/*
 
 	Skip interface

*/

#if !defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_SKIP_INT

#define CHARSET_NL 		L"\x0a\x0d"
#define CHARSET_WS 		L"\x20\t"
#define CHARSET_PUNCT     	L".,;:!?()[]{}"

// TODO:
axres skip_until_r(
	_in const c16		*text,
	_in const c16		*charset,
	_out const c16		**loc
);

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
axres read_line(
	_in const c16 		*text,
	_in_out u64		*size,
	_in_out _eval c16	*buf // Evaluate by using (size * sizeof(c16))
);
axres read_word(
	_in const c16 		*text,
	_in_out u64		*size,
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
axres c_split_by(
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

#if !defined(AX_PARSER_FILE_INT)
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

#endif // !defined(AX_PARSER_FILE_INT)

#endif // !defined(AX_PARSER_INT)

