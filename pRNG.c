#include <stdio.h>
#include "pRNG.h"

uint32_t prng_rotate(uint32_t x, uint32_t k)
{
  return (x << k) | (x >> (32 - k));
}

uint32_t prng_next(prng_t *prng_ctx)
{
  uint32_t e = prng_ctx->a - prng_rotate(prng_ctx->b, 27);
  prng_ctx->a = prng_ctx->b ^ prng_rotate(prng_ctx->c, 17);
  prng_ctx->b = prng_ctx->c + prng_ctx->d;
  prng_ctx->c = prng_ctx->d + e;
  prng_ctx->d = e + prng_ctx->a;
  return prng_ctx->d;
}

void prng_init(prng_t *prng_ctx, uint32_t seed)
{
  uint32_t i;
  prng_ctx->a = 0xf1ea5eed;
  prng_ctx->b = prng_ctx->c = prng_ctx->d = seed;

  for (i = 0; i < 31; ++i)
  {
    (void)prng_next(prng_ctx);
  }
}