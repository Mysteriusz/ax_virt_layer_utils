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

static void ax_print_error(AXSTATUS code, void* address){
	printf("\n%ls %llu \n%ls %p", L"Error with code:", code, L"Recorded at:", address);
	printf("\n%ls\n", L"Metadata:"); 
	printf("\t%ls %llu\n", L"LRESULT", (code & AX_STATUS_LRESULT) == 0 ? 0 : code & ~AX_STATUS_LRESULT); 
	printf("\t%ls %llu\n", L"LERROR", (code & AX_STATUS_LERROR) == 0 ? 0 : code & ~AX_STATUS_LERROR); 
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
