/** @file
	Parser: XXH32 and XXH64, see pa_xxhash.h for the license.
*/

#include "pa_xxhash.h"

volatile const char * IDENT_PA_XXHASH_C="$Id: pa_xxhash.c,v 1.1 2026/11/04 13:40:43 moko Exp $" IDENT_PA_XXHASH_H;

#define ROTL32(x, r) (((x) << (r)) | ((x) >> (32 - (r))))
#define ROTL64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))

// little endian read, not depending on the byte order and alignment
static uint32_t read32(const uint8_t* p) {
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t read64(const uint8_t* p) {
	return (uint64_t)read32(p) | ((uint64_t)read32(p + 4) << 32);
}

#define PRIME32_1 0x9E3779B1U
#define PRIME32_2 0x85EBCA77U
#define PRIME32_3 0xC2B2AE3DU
#define PRIME32_4 0x27D4EB2FU
#define PRIME32_5 0x165667B1U

static uint32_t round32(uint32_t acc, uint32_t input) {
	acc += input * PRIME32_2;
	acc = ROTL32(acc, 13);
	return acc * PRIME32_1;
}

uint32_t pa_xxh32(const void* input, size_t length, uint32_t seed) {
	const uint8_t* p = (const uint8_t*)input;
	const uint8_t* end = p + length;
	uint32_t h;

	if (length >= 16) {
		const uint8_t* limit = end - 16;
		uint32_t v1 = seed + PRIME32_1 + PRIME32_2;
		uint32_t v2 = seed + PRIME32_2;
		uint32_t v3 = seed;
		uint32_t v4 = seed - PRIME32_1;

		do {
			v1 = round32(v1, read32(p)); p += 4;
			v2 = round32(v2, read32(p)); p += 4;
			v3 = round32(v3, read32(p)); p += 4;
			v4 = round32(v4, read32(p)); p += 4;
		} while (p <= limit);

		h = ROTL32(v1, 1) + ROTL32(v2, 7) + ROTL32(v3, 12) + ROTL32(v4, 18);
	} else
		h = seed + PRIME32_5;

	h += (uint32_t)length;

	while (p + 4 <= end) {
		h += read32(p) * PRIME32_3;
		h = ROTL32(h, 17) * PRIME32_4;
		p += 4;
	}
	while (p < end) {
		h += (*p++) * PRIME32_5;
		h = ROTL32(h, 11) * PRIME32_1;
	}

	h ^= h >> 15;
	h *= PRIME32_2;
	h ^= h >> 13;
	h *= PRIME32_3;
	h ^= h >> 16;
	return h;
}

#define PRIME64_1 ((uint64_t)0x9E3779B185EBCA87ULL)
#define PRIME64_2 ((uint64_t)0xC2B2AE3D27D4EB4FULL)
#define PRIME64_3 ((uint64_t)0x165667B19E3779F9ULL)
#define PRIME64_4 ((uint64_t)0x85EBCA77C2B2AE63ULL)
#define PRIME64_5 ((uint64_t)0x27D4EB2F165667C5ULL)

static uint64_t round64(uint64_t acc, uint64_t input) {
	acc += input * PRIME64_2;
	acc = ROTL64(acc, 31);
	return acc * PRIME64_1;
}

static uint64_t merge_round64(uint64_t acc, uint64_t val) {
	acc ^= round64(0, val);
	return acc * PRIME64_1 + PRIME64_4;
}

uint64_t pa_xxh64(const void* input, size_t length, uint64_t seed) {
	const uint8_t* p = (const uint8_t*)input;
	const uint8_t* end = p + length;
	uint64_t h;

	if (length >= 32) {
		const uint8_t* limit = end - 32;
		uint64_t v1 = seed + PRIME64_1 + PRIME64_2;
		uint64_t v2 = seed + PRIME64_2;
		uint64_t v3 = seed;
		uint64_t v4 = seed - PRIME64_1;

		do {
			v1 = round64(v1, read64(p)); p += 8;
			v2 = round64(v2, read64(p)); p += 8;
			v3 = round64(v3, read64(p)); p += 8;
			v4 = round64(v4, read64(p)); p += 8;
		} while (p <= limit);

		h = ROTL64(v1, 1) + ROTL64(v2, 7) + ROTL64(v3, 12) + ROTL64(v4, 18);
		h = merge_round64(h, v1);
		h = merge_round64(h, v2);
		h = merge_round64(h, v3);
		h = merge_round64(h, v4);
	} else
		h = seed + PRIME64_5;

	h += (uint64_t)length;

	while (p + 8 <= end) {
		h ^= round64(0, read64(p));
		h = ROTL64(h, 27) * PRIME64_1 + PRIME64_4;
		p += 8;
	}
	if (p + 4 <= end) {
		h ^= (uint64_t)read32(p) * PRIME64_1;
		h = ROTL64(h, 23) * PRIME64_2 + PRIME64_3;
		p += 4;
	}
	while (p < end) {
		h ^= (*p++) * PRIME64_5;
		h = ROTL64(h, 11) * PRIME64_1;
	}

	h ^= h >> 33;
	h *= PRIME64_2;
	h ^= h >> 29;
	h *= PRIME64_3;
	h ^= h >> 32;
	return h;
}
