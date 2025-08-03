/*

 	BUILD Version 0001 --- Increment on update.

	Module Name:
		ax_error.h

	Abstract:
		This module defines default AX Project`s types, error codes and handlers. 

*/

#if !defined(AX_ERROR_INT)
#define AX_ERROR_INT

typedef uint64_t AXSTATUS;

#define AX_SUCCESS 			0x0000000000000000
#define AX_INVALID_COMMAND	 	0x0000000000000001
#define AX_MEMORY_ERROR 		0x0000000000000002
#define AX_INVALID_ARGUMENT		0x0000000000000003
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

#if defined(AX_UM)

static void ax_log_status(
	AX_IN AXSTATUS 			status,
	AX_IN_OPT uint8_t		metadata,
	AX_IN_OPT const void*		location,
	AX_IN_OPT const wchar_t*	message
){
	static wchar_t meta_buffer[AX_CACHE_SIZE];
	memset(meta_buffer, 0, AX_CACHE_SIZE);

	static wchar_t address_buffer[AX_CACHE_SIZE];
	memset(address_buffer, 0, AX_CACHE_SIZE);

	if (metadata){
		uint64_t lresult = status & ~AX_STATUS_LRESULT;  
		uint64_t lerror = status & ~AX_STATUS_LERROR;  

		swprintf_s(meta_buffer, AX_CACHE_SIZE,
			L"\t%ls --> %llu\n"
			L"\t%ls --> %llu\n",
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
	printf("%ls: %llu\n %ls%ls%ls",
		L"AXSTATUS Status", status,
		meta_buffer,
		address_buffer,
		message == NULL ? L"No message." : message
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
