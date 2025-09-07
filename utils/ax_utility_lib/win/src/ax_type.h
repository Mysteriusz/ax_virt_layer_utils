#if !defined(AX_TYPE_INT)
#define AX_TYPE_INT

#if defined(AX_KM) && defined(AX_UM)
#error "Cannot define both AX_UM and AX_KM"
#elif !defined(AX_UM) && !defined(AX_KM)
#error "Undefined target build mode. (AX_UM/AX_KM)"
#endif

#if defined(AX_WIN32) && defined(AX_LINUX)
#error "Cannot define both AX_WIN32 and AX_LINUX"
#elif !defined(AX_WIN32) && !defined(AX_LINUX)
#error "Undefined target system. (AX_WIN32/AX_LINUX)"
#endif

#if defined(AX_KM)
#define AX_STRICT_BUF_SIZE 
#endif

#if defined(AX_WIN32)

#if defined(AX_UM)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(AX_KM)
#include <wdm.h>

#pragma warning(disable:4820)
#pragma warning(disable:4711)
#pragma warning(disable:5045)
#endif

#endif // defined(AX_WIN32)

#define _in
#define _in_opt

#define _out
#define _out_opt

#define _in_out
#define _in_out_opt

/*
	Indicates that variable HAS to be evaluated by caller POST initial call 

	Example:
		DECL: foo(u32 *size, c16 *buf);

		foo(&size, buffer); // Initial call
		buffer = allocate(size); // POST evaluation may differ depending on the function specs
		foo(&size, buffer); // Recall evaluated
*/
#define _eval 

#if defined(_MSC_VER)
#define __builtin_trap() __debugbreak()
#endif

#define null 		(0)
#define nullptr 	((void*)0)

#define unref(p)	((void)p)
#define addr(v)		(&(long long[]){v})
#define asrt(expr)	((expr == true) ? (void)__builtin_trap() : (void)null)
#define chkf(v,f)	(((v) & (f)) != 0)
#define astp(t,v)	*((t*)((void*)&(v)))

#include <stdbool.h>

#if defined(AX_UM)
#include <stdio.h>
#endif

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

// Characters

#if defined(AX_WIN32)
typedef unsigned char c8; // ansi
typedef unsigned short c16; // unicode
#elif defined(AX_LINUX) 
typedef signed char c8; // ansi
typedef signed short c16; // unicode
#endif

typedef i64 axres;

#endif //!defined(AX_TYPE_INT)

