#include "ax_parser.h"

#if !defined(AX_PARSER_SEQUENCE_INT) && defined(AX_PARSER_COMPLETE)
#define AX_PARSER_SEQUENCE_INT

// Charset of sequence starting identifiers
#define CHARSET_SEQ 			u"<($^?["
typedef struct _fmt_group{
	ax_list *spec_list; // List of _fmt_spec
	ax_list *var_list; // List of _fmt_var
	ax_list *cond_list; // List of _fmt_cond
} fmt_group;

enum spec_type{
	spec_sequence = 0,
	spec_capture_set = 1,
	spec_control_beg = 2,
	spec_control_end = 3,
};
enum spec_mode{
	spec_none = 0,
	spec_optional = 1
};
/*
 	Unstructured linear sequence data holder.
*/
typedef struct _fmt_spec{
	const c16 *value;
	enum spec_type type;
	enum spec_mode mode;
} fmt_spec;

typedef struct _seq_loc{
	const c16 *beg;
	const c16 *end;
} seq_loc;

// Format group character 
#define CAPTURE_FMT_NL		u"{\r}+{\n}"

/*
 	LIFO sequence finder stack  
*/

// Parser fmt_group
axres seq_read_group(
	_in const c16		*fmt,
	_in_out fmt_group 	*grp
);
// Parse and load fmt_group
axres seq_split_fmt(
	_in const c16 		*fmt,
	_in_out fmt_group 	*buf
);
// Locate first match of fmt_group in text
axres seq_locate(
	_in const c16		*text,
	_in fmt_group 		*grp,
	_out seq_loc		*loc
);

/*
 	Known issues:
 	Solved issues:
		- seq: u"|?<{a-c}>?|<{d-f}>]"
		- text: u"bef|def]aft"
		Result is AX_NOT_FND.
		Cause and possible fix:
			Current setup prohibits usage of 2 or more capture group one after another.
			That makes the next search when not found the `physical sequence` return AX_NOT_FND
			since there is no seq_end for the capture set to invalidate the range.
			Could be fixed by not only iterating text for next but also iterating all next`s to find the seq_end.
		- seq: u"[<{a-z}>|<{.}>]:"
		- text: u"l[section|other_text||a|b]:"
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
#if defined(AX_PARSER_FILE_INT)
axres seq_find_f(
	_in io_file		*file,
	_in const c16 		*fmt
);
axres seq_find_all_f(
	_in io_file		*file,
	_in const c16 		*fmt,
	_in_out ax_list		*locs // Access by index_as(locs, 0, seq_loc*)
);
#endif // defined(AX_PARSER_FILE_INT)


#define CAPTURE_FMT_ASCII	u"{\x20-\x7f}"

/*
 	SEQUENCE INTERNAL UTILITIES
*/

// ====================== CHARSET ======================

// Syntax invalidation for seq_cap_to_charset
bool seq_cap_to_charset_inv(
	_in const c16		*cap // cap FOR capture group
);
/*
	Rules:
   	Input of cap has to start with:
		L'{'
	and end with:
		L'}'
	Required input is a math set equation.
	Syntax is char-sensitive.
	Syntax doesn`t allow spaces.

	Input example:
		{.}
		{a-z}-{x-z}+{_}
		{l-n}+{x}
		{\x20-\x45}+{\x6f}

	In case you want to include syntax characters -> {, }, -, +, etc...
	You either have to include them in the capture range like above
	OR you can add them by adding single character like so:
		{a-z}+{{}+{-}+{<}
	
	Returns HEAP ALLOCATED condition structure
*/
_free const c16 *_seq_cap_to_charset(
	_in const c16		*cap // cap FOR capture group
);

/*
 	Locate ending of the capture set sequence
*/
axres seq_group_cap_end(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out const c16		**loc
);

/*
 	Capture set parsing.

	Input:
		u"<{a-k}-{d}>"
	Output:
		Parsed capture set in form of a string u"abcefghijk" in buf
*/
axres seq_group_cap(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_out const c16		**loc,
	_out fmt_spec		*buf
);

/*
 	Match string range with charset occurence.

	Input:
		u"(c16[]){abcdefg}"
	Output:
		AX_SUCC if matches. 
*/
axres seq_cap_match(
	_in const c16		*text,
	_in const c16		*cap,
	_in const c16 		*seq_beg,
	_in_opt const c16 	*seq_end
);

// ====================== FUNCTION ======================

enum cond_state{
	outside = 0,
	inside = 1,
};
/*
 	Condition definition sequence structure.
*/
typedef struct _fmt_cond{
	bool ret;
	enum cond_state state;
	const c16 *bef; // Before . character
	const c16 *aft; // After . character
} fmt_cond;

#define SEQ_COND_CHARSET u"."

/* 
	Function has to consist of at least one control character.
	Recognized characters:
		. - Any character
*/
bool seq_func_to_cond_inv(
	_in const c16		*func // func for function
);
/*
	Rules:
	Syntax is char-sensitive.
	Syntax doesn`t allow spaces.

	Input example:
		+:[[$]]
		!:<$>
		!::$>:
	
	Returns HEAP ALLOCATED condition structure
*/
_free fmt_cond *_seq_func_to_cond(
	_in const c16		*func // func for function
);

axres seq_group_cond_end(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out const c16		**loc
);
axres seq_group_cond(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*cond_list,
	_out const c16		**loc
);
axres seq_condition_match(
	_in const c16 		*text,
	_in const c16		*text_char,
	_in fmt_cond 		*cond,
	_out const c16		**loc
);

// ====================== VARIABLE ======================

enum var_type{
	type_unk = 0,
	type_u8 = 1,
	type_u16 = 2,
	type_u32 = 4,
	type_u64 = 8,
};
/*
 	Variable definition sequence structure.

	When processing string variable occurences only capture output of following specifiers:
		- spec_sequence
		- spec_capture_set
*/
typedef struct _fmt_var{
	/*
	 	Fields influenced by syntax.
	*/
	const c16 *name;
	enum var_type type;
	u32 length;
	u8 span;

	/*
	 	Fields used internally.
	*/
	bool collect; // Collecting values into value buffer
	u32 spec_i; // Initial index where output gather begins
	c16 *value; // Value inside of the searched processed text
} fmt_var;

/* 
	Variable has to consist of at least n: field which indicated the name.
	All fields are to be separated by L';' character.
	Field list:
		n: -> name
		TODO
		l: -> value length in characters
		
		TODO
		IMPORTANT!!!
 		(This will influence l: field if not defined to type_size * sizeof(c16))
		(If l: field is bigger than the type error will occur)
		t: -> type name (u8, u16, u32, u64, str)

		TODO
		(By default span is always equal to 1)
		s: -> span over n-count of capture group match values (MAX == 255)
*/
bool seq_label_to_var_inv(
	_in const c16		*label
);
/*
	Rules:
	Syntax is char-sensitive.
	Syntax doesn`t allow spaces.

	Input example:
		n:key
		n:key;s:10
		n:key;s:10;t:u32
		n:key;s:10;l:4
	
	Returns HEAP ALLOCATED variable meta structure
*/
_free fmt_var *_seq_label_to_var(
	_in const c16		*label,
	_in u32 		spec_i
);

axres seq_group_var_end(
	_in const c16 		*fmt,
	_in const c16 		*fmt_char,
	_out const c16		**loc
);
axres seq_group_var(
	_in const c16		*fmt,
	_in const c16		*fmt_char,
	_in ax_list 		*var_list,
	_in u32			spec_i,
	_out const c16		**loc
);
axres seq_var_process(
	_in ax_list		*var_list,
	_in u32			match_i,
	_in c16			*match_res // Allocated result of the specifier at index spec_i
);

#endif

