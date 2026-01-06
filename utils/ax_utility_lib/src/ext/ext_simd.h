/*
 	Simplifies and unifies AVX support into multiple base types
*/

#if !defined(EXT_SIMD)
#define EXT_SIMD

#include <immintrin.h>

#if defined(__AVX512__) || defined(AX_AVX512)
#define AX_AVX2

#if !defined(EXT_SIMD_MAX)
#define EXT_SIMD_MAX
typedef __m512i simd_imax;
#define simd_imax_mask(b) {(b), (b), (b), (b), (b), (b), (b), (b)} __attribute__((vector_size(64)))
#endif

#endif

#if defined(__AVX2__) || defined(AX_AVX2) 
#define AX_AVX

#if !defined(EXT_SIMD_MAX)
#define EXT_SIMD_MAX

typedef __m256i simd_imax;
#define SIMD_IMAX_BLOCK_ALIGN 		32
#define SIMD_IMAX_BLOCK_COUNT 		8
#define SIMD_IMAX_BIT_WIDTH  		256

#define simd_imax_call(op, type, ...) 	(_mm256_##op##_##type(__VA_ARGS__))
#define simd_imax_prim(i, b) 		((simd_imax)(_mm256_insert_epi32(simd_imax_zero, (unsigned long)(b), (i))))
#define simd_imax_mask(b) 		((simd_imax)_mm256_set1_epi32((b)))

#define simd_imax_sub(a, b) 	(_mm256_sub_epi64(a, b))
#define simd_imax_add(a, b) 	(_mm256_add_epi64(a, b))
#define simd_imax_and(a, b) 	(_mm256_and_si256(a, b))
#define simd_imax_mul(a, b) 	(_mm256_mul_epi32(a, b))
#define simd_imax_xor(a, b) 	(_mm256_xor_si256(a, b))
#define simd_imax_or(a, b) 	(_mm256_or_si256(a, b))
#define simd_imax_not(a) 	(_mm256_xor_si256(a, simd_imax_mask(0xFFFFFFFFUL)))
#define simd_imax_btor(a) 	(_mm256_set1_epi8(a))
#define simd_imax_zero 		(_mm256_setzero_si256())
#define simd_imax_cmpr(a, b) 	((simd_imax)_mm256_cmpeq_epi64(a, b))
#define simd_imax_store(a, b) 	(_mm256_storeu_si256((simd_imax*)a, b))
#define simd_imax_cmp(a, b) 	({ \
	__m256i (t) = _mm256_cmpeq_epi64(a, b); \
	(bool)_mm256_extract_epi64((t), 0) && _mm256_extract_epi64((t), 1) && _mm256_extract_epi64((t), 2) && _mm256_extract_epi64((t), 3); \
})

#endif // !defined(EXT_SIMD_MAX)

#else
// NO AVX SUPPORT

typedef unsigned long long simd_imax;

#endif // defined(__AVX512__)

inline unsigned long simd_imax_ctz(
	simd_imax a
){ 
	simd_imax temp = simd_imax_call(cmpeq, epi32, a, simd_imax_zero);
	/*
	 	Mask all blocks that are zero

		For blocks of 2 bits (only for showcase)
		Ex: 
			a = 0b1010001
			temp = (a == 0b00000000)

			Result of temp will be maximum value (0b11) of the block if matching with zerod block
			- So in this case temp = 0b00001100
			Now we can check which of the MSB bit in all blocks are set to one and get the block-count bit mask
			- That will result in 0b0010 (Each bit represents one block)
			Inverting them will check if all blocks are equal to 0
			If after invertin the mask we will get all zeros that means the the entire mask is equal to 0
			- In this case resullt of ~0b0010 is 0b1101

			Case with all zeros:
			a = 0b0000000
			temp = (a == 0b00000000) = 0b11111111
			mask = ~0b1111 = 0b0000
			
	*/
	unsigned long mask = ~simd_imax_call(movemask, epi8, temp); 
	if (mask == 0) {
		return SIMD_IMAX_BIT_WIDTH;
	}

	// Get all blocks that are zero
	unsigned long trail = __builtin_ctzl(mask) * 8;

	// Load register to buffer
	alignas(SIMD_IMAX_BLOCK_ALIGN)
		unsigned long buf[SIMD_IMAX_BLOCK_COUNT];
	simd_imax_store(buf, a);

	// Ceil to block count to get index of the last block
	trail += __builtin_ctzl(buf[(trail + SIMD_IMAX_BLOCK_ALIGN - 1) / SIMD_IMAX_BLOCK_ALIGN]);
	return trail;
}

#endif // !defined(EXT_SIMD)

