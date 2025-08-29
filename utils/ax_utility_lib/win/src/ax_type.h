#if !defined(AX_TYPE_INT)
#define AX_TYPE_INT

#include <stdio.h>
#include <stdbool.h>

#if defined(AX_WIN32)
#include <windows.h>
#endif

#define _in
#define _in_opt

#define _out
#define _out_opt

#define _in_out
#define _in_out_opt

#define _eval

#define null 		(0)
#define nullptr 	((void*)0)

#define unref(p)	((void)p)

// Numerical 

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef union{
	i64 sig_64;
	u64 unsig_64;

	i32 sig_32;
	u32 unsig_32;

	i16 sig_16;
	u16 unsig_16;

	i8 sig_8;
	u8 unsig_8;
} iu64;

#define chkf(v,f)	(((v) & (f)) != 0)

// Characters

#if defined(AX_WIN32)
typedef unsigned char c8; // ansi
typedef unsigned short c16; // unicode
#elif defined(AX_LIN) 
typedef signed char c8; // ansi
typedef signed short c16; // unicode
#endif

typedef i64 axres;

#endif //!defined(AX_TYPE_INT)

