#if !defined(AX_TYPE_INT)
#define AX_TYPE_INT

#if defined(AX_KM) && defined(AX_UM)
#error "Cannot define both AX_UM and AX_KM"
#elif !defined(AX_UM) && !defined(AX_KM)
#error "Undefined target build mode. (AX_UM/AX_KM)"
#endif

#if defined(AX_WIN64) && defined(AX_LINUX)
#error "Cannot define both AX_WIN64 and AX_LINUX"
#elif !defined(AX_WIN64) && !defined(AX_LINUX)
#error "Undefined target platform. (AX_WIN64/AX_LINUX)"
#endif

#if defined(AX_KM)
#ifndef AX_STRICT_BUF_SIZE
#define AX_STRICT_BUF_SIZE 

#define AX_MODE KM

#endif
#elif defined(AX_UM)

#define AX_MODE UM

#endif // defined(AX_KM)

#if defined(AX_WIN64)

#define AX_PLATFORM WIN64

/*
 	Windows specific architecture macros
*/
#if defined(__x86_64__) && !defined(__aarch64__)

#ifndef _AMD64_ 
#define _AMD64_
#endif

#elif !defined(__x86_64__) && defined(__aarch64__)

#ifndef _ARM64_ 
#define _ARM64_
#endif

#else
#error "Non 64-bit architectures currently arent supported."
#endif // defined(__x86_64__) && !defined(__aarch64__)

#if defined(AX_UM)
#include <heapapi.h>
#include <handleapi.h>
#include <winreg.h>
#include <winerror.h>
#include <memoryapi.h>
#elif defined(AX_KM)
#include <wdm.h>

#pragma warning(disable:4820)
#pragma warning(disable:4711)
#pragma warning(disable:5045)
#endif

#elif defined(AX_LINUX)

#define AX_PLATFORM LINUX

#endif // defined(AX_WIN64)

#define EXPAND(a,b) a##b
#define CAT(a,b) EXPAND(a,b)

#define _opt

#define _in
#define _in_opt

#define _out
#define _out_opt

#define _in_out
#define _in_out_opt

/*
	Indicates that variable HAS to be evaluated by caller POST initial call.

	Example:
		DECL: foo(u32 *size, c16 *buf);

		foo(&size, buffer); // Initial call
		buffer = allocate(size); // POST evaluation may differ depending on the function specs
		foo(&size, buffer); // Recall evaluated
*/
#define _eval 

/*
 	Auto pass indicator.
	Means the value passed won`t be null and can be used whenever you want.

	Only applicable to pointer types.
*/
#define _prepass [static 1]

/*
 	Internally heap allocated variable indicator (freeable).
	Means the value is stored on the heap and freeable if internal call succeded.

	Only applicable to pointer types.
*/
#define _free

// MSVC compiler
#if defined(_MSC_VER)
#define __builtin_trap() __debugbreak()
#define _inline_force __forceinline
// GCC/Clang compiler
#elif
#define _inline_force __attribute__((always_inline))
#endif

#define null 		(0)
#define nullptr 	((void*)0)

// Declare unreferenced parameter (p)
#define unref(p)	((void)p)
// Address of the value (v)
#define addr(v)		(&(long long[]){v})
// Multi-platform assertion (expr)
#define asrt(expr)	(((expr) == false) ? (void)__builtin_trap() : (void)null) 
// Check bit flag (f) in the value (v)
#define chkf(v,f)	(((v) & (f)) != 0) 
// Cast value (v) to type (t)
#define astp(t,v)	*((t*)((void*)&(v))) 
// Count digit count in an intiger (i)
#define cntd(i)		((u32)log_b((i), 10) + 1)
// Set (n)-th bit to 1
#define bit(n)		(1 << (n))
// Offset of field (f) in the structure (s)
#define offo(f,s)	((u32)&(((s*)nullptr)->f))
// Offset of field (f) in the structure (s) computed directly
#define offo_n(f,s)	((u32)&((s)->f) - (u64)(s))

#include <stdbool.h>
#include "ext_math.h"

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

#if defined(AX_WIN64)
typedef unsigned char c8; // ansi
typedef unsigned short c16; // utf-16
#elif defined(AX_LINUX) 
typedef signed char c8; // ansi
typedef signed short c16; // utf-16
#endif

typedef u64 axres;

// axres breakdown structure
typedef struct _axres_s{ 
	// 32-bit block

	u16 err : 12; // MAX 4095 (0xFFF)
	u16 meta : 4; // Flag per bit
	u16 reseverd0;

	// 32-bit block

	u32 reserved1 : 32;
} axres_s;

#endif //!defined(AX_TYPE_INT)

