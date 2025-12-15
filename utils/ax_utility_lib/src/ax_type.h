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

#define AX_MODE AXKM

#endif // AX_STRICT_BUF_SIZE
#elif defined(AX_UM)

#define AX_MODE AXUM

#endif // defined(AX_KM)

#if defined(AX_WIN64)

#define AX_PLATFORM AXWIN64

/*
 	Windows specific architecture macros
*/
#if (defined(__x86_64__) || defined(_WIN64)) && !defined(__aarch64__)

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
#include <fileapi.h>
#include <handleapi.h>
#include <winreg.h>
#include <winerror.h>
#include <memoryapi.h>
#include <errhandlingapi.h>
#elif defined(AX_KM)
#include <ntifs.h>

/*
	Function may or may not return NTSTATUS meta code
*/
#define _ntstatus_axres

#pragma warning(disable:4820)
#pragma warning(disable:4711)
#pragma warning(disable:5045)
#endif

#elif defined(AX_LINUX)

#define AX_PLATFORM AXLINUX

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
		DECL: foo(u32 *size, _eval c16 *buf);

		foo(&size, buffer); Initial call
		buffer = allocate(size); POST evaluation may differ depending on the function specs
		foo(&size, buffer); Recall evaluated
*/
#define _eval 

/*
 	Auto pass indicator.
	Means the variable passed won`t be null and can be used without assertions
	AND the variable should not be freed.

	Only applicable to pointer types.
*/
#if defined(_MSC_VER)
#define _prepass [1]
#else
#define _prepass [static 1]
#endif

/*
 	Internally heap allocated variable indicator (freeable).
	Means the variable is stored on the heap and is freeable if internal call succeded.

	Only applicable to pointer types.
*/
#define _free

// MSVC compiler
#if defined(_MSC_VER)

#define __builtin_trap() __debugbreak()
#define _inline_force __forceinline
#define _inline_avert __declspec(noinline)

// GCC/Clang compiler
#elif defined(__clang__)

#define _inline_force __attribute__((always_inline))
#define _inline_avert __attribute__((noinline))

#endif

#define null 		(0)
#define nullptr 	((void*)0)

// Declare unreferenced parameter (p)
#define unref(p)	((void)p)
// Address of the value (v)
#define addr(v)		(&(long long[]){v})
// Runtime assertion (expr)
#define asrt(expr)	(((expr) == false) ? (void)__builtin_trap() : (void)null)
// Check bit mask (m) in the value (v)
#define chkf(v,m)	(((v) & (m)) != 0) 
// Cast value (v) to type (t)
#define astp(t,v)	(*((t*)((void*)&(v))))
// Count digit count in an intger (i)
#define cntd(i)		((u32)log_b((i), 10) + 1)
// Set (n)-th bit to 1
#define bit(n)		(1 << (n))
// Offset of field (f) in the structure (s)
#define offo(f,s)	((u32)&(((s*)nullptr)->f))
// Offset of field (f) in the structure (s) computed directly
#define offo_n(f,s)	((u32)&((s)->f) - (u64)(s))
// Value as (n) Gigibytes
#define GB(n)		((u64)(1 << 30) * (n))

#define AX_BIG_ENDIAN 		1
#define AX_LIT_ENDIAN 		2

#define AX_UNK_ENDIAN 		\
	((addr(0x01ULL << 56)[0] == 0) \
	 ? AX_BIG_ENDIAN \
	 : AX_LIT_ENDIAN)

#define value_in_range(v, bw, e) \
	(((e) == AX_BIG_ENDIAN) \
		? ((v) << ((bw) - 1)) /* AX_BIG_ENDIAN */ \
		: ((v) >> ((bw) - 1))) /* AX_LIT_ENDIAN */ \

#include <stdbool.h>
#include "ext_math.h"

#if defined(AX_UM)
#include <stdio.h>
#include <stdlib.h>
#endif

/*
 	Numerical
*/ 

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// Universal number
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

/*
 	Characters
*/

typedef unsigned char c8; // ansi
typedef unsigned short c16; // utf-16

// Default result code type
typedef u64 axres;

// axres breakdown structure
typedef struct _axres_s{ 
	// 32-bit block

	u16 err : 12; // max 4095 (0xfff)
	u16 reserved0 : 4;

	struct {
		u16 reserved1 : 15;
		u16 ntstatus : 1;
	} meta;

	// 32-bit block

	/*
		Meta code may or may not be null,
		and to ensure it`s checked the program should set appropriate checks
		in the meta struct
	*/
	u32 meta_err : 32;
} axres_s;

#endif //!defined(AX_TYPE_INT)

