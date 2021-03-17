#ifndef _PRNG_H__
#define _PRNG_H__

#include <stdint.h>
#include <stdio.h>

typedef struct
{
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
} prng_t;

// static prng_t prng_ctx;
uint32_t prng_rotate(uint32_t x, uint32_t k);
uint32_t prng_next(prng_t *prng_ctx);
void prng_init(prng_t *prng_ctx, uint32_t seed);

#endif /* #ifndef _PRNG_H__ */