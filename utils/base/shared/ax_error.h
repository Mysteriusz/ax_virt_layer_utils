/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_error.h

	Abstract:
		This module defines default AX Project`s types, error codes and handlers. 

*/

#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

#include "stdbool.h"

typedef uint64_t AXSTATUS;

#define AX_SUCCESS 			0x0000000000000000
#define AX_INVALID_COMMAND	 	0x0000000000000001
#define AX_MEMORY_ERROR 		0x0000000000000002
#define AX_INVALID_ARGUMENT		0x0000000000000003
#define AX_INVALID_DATA			0x0000000000000004
#define AX_EXECUTION_ERROR		0x0000000000000005
#define AX_NOT_FOUND			0x0000000000000006
#define AX_ACCESS_VIOLATION		0x0000000000000007
#define AX_BUFFER_TOO_SMALL		0x0000000000000010
#define AX_BUFFER_TOO_BIG		0x0000000000000011
#define AX_INVALID_BUFFER_SIZE		0x0000000000000012
#define AX_INVALID_STACK		0x0000000000000013
#define AX_INVALID_STACK_SIZE		0x0000000000000014
#define AX_NOT_IMPLEMENTED		0x00000000ffffffff
// AX Error check 
#define AX_ERROR(code)			((code != AX_SUCCESS))

#define AX_STATUS_LRESULT 		0x8000000000000000
#define AX_STATUS_LERROR 		0x4000000000000000
// AX Windows error check (Is error has a Windows flag)
#define AX_WERROR(code)			((code & AX_STATUS_LRESULT) || (code & AX_STATUS_LERROR))

// AX Linux error check (Is error has a Linux flag)
#define AX_LERROR(code)			((code & AX_STATUS_LRESULT) || (code & AX_STATUS_LERROR))

// AX Native error check (Is error is purely from AX)
#define AX_NERROR(code)			(!AX_WERROR(code) && !AX_LERROR(code))

#define AX_STATUS_FLAGS (AX_STATUS_LRESULT | AX_STATUS_LERROR)

#if defined(AX_UM)

static void ax_log_status(
	AX_IN AXSTATUS 			status,
	AX_IN_OPT bool			metadata,
	AX_IN_OPT const void*		location,
	AX_IN_OPT const wchar_t*	message
){
	static wchar_t meta_buffer[AX_CACHE_SIZE];
	memset(meta_buffer, 0, AX_CACHE_SIZE);

	static wchar_t address_buffer[AX_CACHE_SIZE];
	memset(address_buffer, 0, AX_CACHE_SIZE);

	if (metadata){
		wchar_t* lresult = status & AX_STATUS_LRESULT ? L"TRUE" : L"FALSE";  
		wchar_t* lerror = status & AX_STATUS_LERROR ? L"TRUE" : L"FALSE";  

		swprintf_s(meta_buffer, AX_CACHE_SIZE,
			L"\t%ls --> %ls\n"
			L"\t%ls --> %ls\n",
			L"LRESULT", lresult,
			L"LERROR", lerror
		);
	}
	if (location != NULL){
		swprintf_s(address_buffer, AX_CACHE_SIZE,
			L"%ls --> %p\n",
			L"Error at", location
		);
	}

	// Print status
	printf("\r%ls: %llu\n\r%ls: %llu\n %ls%ls%ls\n",
		L"AXSTATUS Status", status,
		L"AXSTATUS Raw", status & ~AX_STATUS_FLAGS,
		meta_buffer,
		address_buffer,
		message == NULL ? L"\rNo message." : message
	);
}

#if defined(_MSC_VER)
	#define AX_NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || (__clang__) 
	#define AX_NORETURN __attribute__(noreturn)	
#endif 

AX_NORETURN static void ax_crash(
	void
){
	abort();
}

#endif // AX_UM

#endif // AX_ERROR_INT
