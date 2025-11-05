#include "ax_type.h"

/*
 	
   	Murmur3 hashing algorithm implemented from:

		https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.h

*/

inline u32 rotl32 (u32 x, u8 r){
  	return (x << r) | (x >> (32 - r));
}
inline u64 rotl64 (u64 x, u8 r){
	return (x << r) | (x >> (64 - r));
}
#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

u64 murmur_x64_128(const u8 *key, u64 size, u64 seed);

