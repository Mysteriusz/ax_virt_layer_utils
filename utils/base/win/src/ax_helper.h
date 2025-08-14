#if !defined(AX_HELPER_INT)
#define AX_HELPER_INT

#include "ax_utility.h"

static inline size_t _ax_size_w(
	AX_IN const wchar_t*	string	
){
	return (wcslen(string) + 1) * sizeof(wchar_t);
}
static inline size_t _ax_size_wf(
	AX_IN const wchar_t*	string	
){
	return wcslen(string) + 1;
}

static inline void _ax_print_s(
	AX_IN size_t		value	
){
	printf("%zi\n", value);
}
static inline void _ax_print_w(
	AX_IN const wchar_t*	string
){
	printf("%ls\n", string);
}

/*

 	Allocate a new buffer and load working directory.

*/
wchar_t* _ax_load_working_directory(
	void
);

/*

 	Allocate and expand environment variables if used in the path.

	Example: 
		%USERPROFILE%\path -> C:\users\username\path

*/
wchar_t* _ax_expand_path(
	AX_IN wchar_t*  		path
);

wchar_t* _ax_get_file_path(
	AX_IN HANDLE 			file
);
#endif // !defined(AX_HELPER_INT)

