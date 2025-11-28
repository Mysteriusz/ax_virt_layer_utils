#include "murmur.h"

_inline_force u64 getblock64(const u64 *p, i32 i){
  	return p[i];
}
_inline_force u32 fmix32 (u32 h){
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

_inline_force u64 fmix64 (u64 k){
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdLLU;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53LLU;
	k ^= k >> 33;

	return k;
}
u64 murmur_x64_128(const u8 *key, u64 size, u64 seed){
	const u8 * data = (const u8*)key;
	const i32 nblocks = size / 16;

	u64 h1 = seed;
	u64 h2 = seed;

	const u64 c1 = 0x87c37b91114253d5LLU;
	const u64 c2 = 0x4cf5ad432745937fLLU;

	//----------
	// body

	const u64 * blocks = (const u64 *)(data);

	for(i32 i = 0; i < nblocks; i++)
	{
		u64 k1 = getblock64(blocks,i*2+0);
		u64 k2 = getblock64(blocks,i*2+1);

		k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

		h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

		k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

		h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
	}

	//----------
	// tail

	const u8 * tail = (const u8*)(data + nblocks*16);

	u64 k1 = 0;
	u64 k2 = 0;

	switch(size & 15)
	{
	case 15: k2 ^= ((u64)tail[14]) << 48;
	case 14: k2 ^= ((u64)tail[13]) << 40;
	case 13: k2 ^= ((u64)tail[12]) << 32;
	case 12: k2 ^= ((u64)tail[11]) << 24;
	case 11: k2 ^= ((u64)tail[10]) << 16;
	case 10: k2 ^= ((u64)tail[ 9]) << 8;
	case  9: k2 ^= ((u64)tail[ 8]) << 0;
	         k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

	case  8: k1 ^= ((u64)tail[ 7]) << 56;
	case  7: k1 ^= ((u64)tail[ 6]) << 48;
	case  6: k1 ^= ((u64)tail[ 5]) << 40;
	case  5: k1 ^= ((u64)tail[ 4]) << 32;
	case  4: k1 ^= ((u64)tail[ 3]) << 24;
	case  3: k1 ^= ((u64)tail[ 2]) << 16;
	case  2: k1 ^= ((u64)tail[ 1]) << 8;
	case  1: k1 ^= ((u64)tail[ 0]) << 0;
	         k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= size; h2 ^= size; 

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	h2 += h1;

	return h1;
}

