/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random-inline.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 128
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

#define POS1 89
#define SL1 18
#define SL2 1
#define SR1 3
#define SR2 2
#define MSK1 0x7f7fffffU
#define MSK2 0xfffbffffU
#define MSK3 0xeffffbffU
#define MSK4 0xfefe7befU

struct W128_T {
    uint32_t a[4];
};

typedef struct W128_T w128_t;

static w128_t sfmt[N + 1];
static uint32_t *psfmt = (uint32_t *)sfmt;
static int idx;
static const uint32_t mask[4] = {MSK1, MSK2, MSK3, MSK4};

INLINE unsigned int get_rnd_maxdegree(void)
{
    return MAXDEGREE;
}

INLINE unsigned int get_rnd_mexp(void)
{
    return MEXP;
}

INLINE unsigned int get_onetime_rnds(void) {
    return N * 4;
}

INLINE void print_param(FILE *fp) {
    fprintf(fp, "POS1 = %u\n", POS1);
    fprintf(fp, "SL1 = %u\n", SL1);
    fprintf(fp, "SL2 = %u\n", SL2);
    fprintf(fp, "SR1 = %u\n", SR1);
    fprintf(fp, "SR2 = %u\n", SR2);
    fprintf(fp, "MSK1 = %08x\n", MSK1);
    fprintf(fp, "MSK2 = %08x\n", MSK2);
    fprintf(fp, "MSK3 = %08x\n", MSK3);
    fprintf(fp, "MSK4 = %08x\n", MSK4);
    fflush(fp);
}

INLINE void print_param2(FILE *fp) {
    fprintf(fp, "[POS1, SL1, SL2, SL3, SL4, SL5, SL6, SL7, SL8,"
	    " SR1, SR2, SR3, SR4] = "
	    "[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u]\n", 
	    POS1, SL1, SR1, MSK1, MSK2, MSK3, MSK4, 0, 0, 
	    0, 0, 0, 0);
    fflush(fp);
}

INLINE void print_state(FILE *fp) {
    int i, j;
    for (i = 0; i < N; i++) {
	for (j = 0; j < 4; j++) {
	    fprintf(fp, "%08x ", sfmt[i].a[j]);
	}
	if (i % 2 == 1) {
	    fprintf(fp, "\n");
	}
    }
}

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
void lshift128(w128_t *out, const w128_t *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->a[3] << 32) | ((uint64_t)in->a[2]);
    tl = ((uint64_t)in->a[1] << 32) | ((uint64_t)in->a[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->a[0] = (uint32_t)ol;
    out->a[1] = (uint32_t)(ol >> 32);
    out->a[2] = (uint32_t)oh;
    out->a[3] = (uint32_t)(oh >> 32);
}

INLINE static
#if defined(__GNUC__)
__attribute__((always_inline)) 
#endif
    void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *d) {
    w128_t x;

    lshift128(&x, a, SL2);
    r->a[0] = a->a[0] ^ x.a[0] ^ ((b->a[0] >> SR1) & mask[0]) ^ (c->a[0] >> SR2)
	^ (c->a[0] << SL1) ^ d->a[3];
    r->a[1] = a->a[1] ^ x.a[1] ^ ((b->a[1] >> SR1) & mask[1]) ^ (c->a[1] >> SR2)
	^ (c->a[1] << SL1) ^ d->a[2];
    r->a[2] = a->a[2] ^ x.a[2] ^ ((b->a[2] >> SR1) & mask[2]) ^ (c->a[2] >> SR2)
	^ (c->a[2] << SL1) ^ d->a[0];
    r->a[3] = a->a[3] ^ x.a[3] ^ ((b->a[3] >> SR1) & mask[3]) ^ (c->a[3] >> SR2)
	^ (c->a[3] << SL1) ^ d->a[1];
}

INLINE static void xor128(w128_t *to, w128_t *from) {
	to->a[0] ^= from->a[0];
	to->a[1] ^= from->a[1];
	to->a[2] ^= from->a[2];
	to->a[3] ^= from->a[3];
}

INLINE static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&sfmt[0], &sfmt[0], &sfmt[POS1], &sfmt[N -1], &lung);
    xor128(&lung, &sfmt[0]);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], &sfmt[i - 1], &lung);
	xor128(&lung, &sfmt[i]);
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N], &sfmt[i - 1],
		     &lung);
	xor128(&lung, &sfmt[i]);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&array[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 1], &lung);
    xor128(&lung, &array[0]);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], &array[i - 1],
		     &lung);
	xor128(&lung, &array[i]);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	xor128(&lung, &array[i]);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	xor128(&lung, &array[i]);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	xor128(&lung, &array[i]);
	sfmt[j] = array[i];
    }
    sfmt[N] = lung;
}

INLINE uint32_t gen_rand(void)
{
    uint32_t r;

    if (idx >= N * 4) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

void fill_array(uint32_t array[], int size)
{
    assert(size % 4 == 0);
    assert(size >= 2 * N * 4);
    gen_rand_array((w128_t *)array, size / 4);
}

INLINE void init_gen_rand(uint32_t seed)
{
    int i;

    psfmt[0] = seed;
    for (i = 1; i < (N + 1) * 4; i++) {
	psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
    }
    idx = N * 4;
}

#ifdef TICK
#include "test_time_inline.c"
#else
#include "test_time2_inline.c"
#endif