
#ifndef SKUNK_H__
#define SKUNK_H__

#include "structs/uint256.h"
#include "structs/uint512.h"

#include "crypto/common/sph_skein.h"
#include "crypto/common/sph_cubehash.h"
#include "crypto/common/sph_types.h"
#include "crypto/skunk/sph_fugue.h"
#include "crypto/skunk/sph_gost.h"

/* ----------- Skunk Hash ------------------------------------------------ */
template<typename T1>
inline uint256 SkunkHash(const T1 pbegin, const T1 pend)

{
	sph_skein512_context	 ctx_skein;
	sph_cubehash512_context  ctx_cubehash;
	sph_fugue512_context	 ctx_fugue;
	sph_gost512_context 	 ctx_gost;
	static unsigned char	 pblank[1];

	uint512 hash[17];

	sph_skein512_init(&ctx_skein);
	sph_skein512 (&ctx_skein, (pbegin == pend ? pblank : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]));
	sph_skein512_close(&ctx_skein, static_cast<void*>(&hash[0]));

	sph_cubehash512_init(&ctx_cubehash);
	sph_cubehash512 (&ctx_cubehash, static_cast<const void*>(&hash[0]), 64);
	sph_cubehash512_close(&ctx_cubehash, static_cast<void*>(&hash[1]));

	sph_fugue512_init(&ctx_fugue);
	sph_fugue512 (&ctx_fugue, static_cast<const void*>(&hash[1]), 64);
	sph_fugue512_close(&ctx_fugue, static_cast<void*>(&hash[2]));

	sph_gost512_init(&ctx_gost);
	sph_gost512 (&ctx_gost, static_cast<const void*>(&hash[2]), 64);
	sph_gost512_close(&ctx_gost, static_cast<void*>(&hash[3]));

	return hash[3].trim256();
}


#endif
