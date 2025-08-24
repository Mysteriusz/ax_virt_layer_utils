#if !defined(AX_PARSER_INT)
#define AX_PARSER_INT 

#include "ax_type.h"
#include "ax_error.h"

#include <string.h>

static u32 _c16len(
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

#define end_c16(tp)		((c16*)(tp + _c16len(tp)))
#define dif_c16(s,e)		(((c16*)e - (c16*)s))
#define in_c16(tp,p)		(dif_c16(tp,p) < _c16len(tp))

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
		&& buf == nullptr){
			return AX_INV_BUF;
		}
	}

*/

/*
 
 	Internal interface

*/

axres starts_with(
	_in const c16		*text,
	_in const c16		*str
);
axres contains(
	_in const c16 		*text,
	_in const c16		value	
);
axres compare(
	_in const c16 		*a,
	_in const c16		*b	
);
axres trim(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u32		*size,
	_in_out c16		**buf
);

/*
 
 	Find interface

*/

#if !defined(AX_PARSER_FIND_INT)
#define AX_PARSER_FIND_INT

axres find_char(
	_in const c16 		*text,
	_in const c16 		val	
);

// TODO: Can be optimized
axres find_substr(
	_in const c16 		*text,
	_in const c16 		*substr,
	_out const c16		**loc 
);

#endif // !defined(AX_PARSER_FIND_INT)

/*
 
 	Skip interface

*/

#if !defined(AX_PARSER_SKIP_INT)
#define AX_PARSER_SKIP_INT

#define CHARSET_NL 		L"\n\r"
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

axres read_until(
	_in const c16 		*text,
	_in const c16		*charset,
	_out u32		*size,
	_in_out c16		**buf
);
axres read_line(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		**buf
);
axres read_word(
	_in const c16 		*text,
	_out u32		*size,
	_in_out c16		**buf
);

axres split_by(
	_in const c16		*text,
	_in const c16		*charset,
	_out u32		*size, // Evaluate by using (size * sizeof(c16*))
	_in_out c16		**buf
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

#endif // !defined(AX_PARSER_INT)

