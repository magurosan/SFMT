/* 
 * Search Parity Check Vector
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

extern "C" {
#include "dsfmt-st.h"
}

#include <NTL/GF2X.h>
#include <NTL/vec_GF2.h>
#include <NTL/mat_GF2.h>
#include "util.h"

NTL_CLIENT;

static int mexp;
static int maxdegree;
static void make_zero_state(dsfmt_t *sfmt, GF2X& poly);
static void test_parity(GF2X& f);
void generating_polynomial104(dsfmt_t *dsfmt, vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree);

void test1(char *filename) {
    dsfmt_t sfmt;
    GF2X minpoly;
    GF2X f;
    GF2X q, rem;
    vec_GF2 vec;
    FILE *fp;
    int i;
    int ok;
    uint64_t fix[2];

    printf("filename:%s\n", filename);
    fp = fopen(filename, "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("test1");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    readFile(f, fp);
    fclose(fp);
    printf("begin test-------\n");
    set_high_const();
    get_fixed(fix);
    printf("fix %016llx\n", fix[0]);
    printf("fix %016llx\n", fix[1]);
    ok = 0;
    for (i = 0; i < 100; i++) {
	init_gen_rand(&sfmt, 123 + i);
	vec.SetLength(2 * maxdegree);
	generating_polynomial104(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	//printf("minpoly = %ld\n", deg(minpoly));
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    //printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    //printf("deg q = %ld\n", deg(q));
	    //printf("NG\n");
	    ;
	} else {
	    //printf("OK\n");
	    ok++;
	}
    }
    printf("end   test ok = %d\n", ok);
}

int main(int argc, char *argv[]) {
    GF2X f;
    FILE *fp;
    uint64_t pcv[2];

    if (argc != 4) {
	printf("usage:%s filename pcv1 pcv2\n", argv[0]);
	exit(1);
    }
    pcv[0] = strtoll(argv[2], NULL, 16);
    pcv[1] = strtoll(argv[3], NULL, 16);
    set_pcv(pcv);
    mexp = get_rnd_mexp();
    printf("mexp = %d\n", mexp);
    maxdegree = get_rnd_maxdegree();
    printf("filename:%s\n", argv[1]);
    test1(argv[1]);
    //return 0;
    fp = fopen(argv[1], "r");
    errno = 0;
    if ((fp == NULL) || errno) {
	perror("main");
	fclose(fp);
	exit(1);
    }
    read_random_param(fp);
    //print_param(stdout);
    readFile(f, fp);
    printf("deg poly = %ld\n", deg(f));
    printBinary(stdout, f);
    fclose(fp);
    test_parity(f);
    return 0;
}

#if 0
void static generating_polynomial104_hi(dsfmt_t *dsfmt, vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree) {
    unsigned int i;
    uint64_t ar[2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand104sp(dsfmt, ar, 0);
	bit = (ar[1] & mask);
	vec[i] = (bit != 0);
    }
}

void static generating_polynomial104_low(dsfmt_t *dsfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree) {
    unsigned int i;
    uint64_t ar[2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	gen_rand104sp(dsfmt, ar, 0);
	bit = (ar[0] & mask);
	vec[i] = (bit != 0);
    }
}
#else
void static generating_polynomial104_hi(dsfmt_t *dsfmt, vec_GF2& vec,
					unsigned int bitpos, 
					unsigned int maxdegree) {
    unsigned int i;
    static uint64_t ar[MAXDEGREE*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    gen_rand104spar(dsfmt, ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][1] & mask);
	vec[i] = (bit != 0);
    }
}

void static generating_polynomial104_low(dsfmt_t *dsfmt, vec_GF2& vec,
					 unsigned int bitpos, 
					 unsigned int maxdegree) {
    unsigned int i;
    static uint64_t ar[MAXDEGREE*2][2];
    uint64_t mask;
    uint64_t bit;

    mask = (uint64_t)1UL << (51 - bitpos);
    gen_rand104spar(dsfmt, ar, 2 * maxdegree);
    for (i = 0; i <= 2 * maxdegree - 1; i++) {
	bit = (ar[i][0] & mask);
	vec[i] = (bit != 0);
    }
}
#endif
void generating_polynomial104(dsfmt_t *dsfmt, vec_GF2& vec, 
			      unsigned int bitpos, 
			      unsigned int maxdegree) {
    if (bitpos < 52) {
	generating_polynomial104_hi(dsfmt, vec, bitpos, maxdegree);
    } else {
	generating_polynomial104_low(dsfmt, vec, bitpos - 52, maxdegree);
    }
}

static void test_parity(GF2X& f) {
    uint64_t fix[2];
    dsfmt_t sfmt;
    GF2X minpoly;
    GF2X q, rem;
    vec_GF2 vec;
    int i;
    int r;

    for (i = 0; i < 10; i++) {
	printf("------\n");
	printf("==shoki\n");
	init_gen_rand(&sfmt, i + 1);
	vec.SetLength(2 * maxdegree);
	generating_polynomial104(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	printf("minpoly = %ld\n", deg(minpoly));
	DivRem(q, rem, minpoly, f);
	if (deg(rem) != -1) {
	    printf("rem != 0 deg rempoly = %ld\n", deg(rem));
	    printf("deg q = %ld\n", deg(q));
	    return;
	}
	r = period_certification(&sfmt);
	if (r == 1) {
	    printf("period certification OK\n");
	} else {
	    printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		printf("error!!\n");
		return;
	    }
	}
	printf("==zero\n");
	get_fixed(fix);
	reset_high_const();
	initial_mask(&sfmt);
	make_zero_state(&sfmt, f);
	generating_polynomial104(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("fail to make zero step 1\n");
	    return;
	}
	sfmt.status[N][0] ^= fix[0];
	sfmt.status[N][1] ^= fix[1];
	set_high_const();
	initial_mask(&sfmt);
	generating_polynomial104(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	if (deg(minpoly) >= mexp) {
	    printf("minpoly = %ld\n", deg(minpoly));
	    printf("fail to make zero step 2\n");
	    return;
	}
	r = period_certification(&sfmt);
	if (r == 1) {
	    printf("period certification OK\n");
	    printf("error OK!\n");
	    return;
	} else {
	    printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		printf("error!!\n");
		return;
	    }
	}
	generating_polynomial104(&sfmt, vec, 0, maxdegree);
	berlekampMassey(minpoly, maxdegree, vec);
	printf("minpoly = %ld\n", deg(minpoly));
	r = period_certification(&sfmt);
	if (r == 1) {
	    printf("period certification OK\n");
	} else {
	    printf("period certification NG -> OK\n");
	    if (!period_certification(&sfmt)) {
		printf("error!!\n");
		return;
	    }
	}
    }
}

#if 0
static void initial_status_parity_check(dsfmt_t *sfmt) {
    int inner = 0;
    int i, j;
    uint64_t work;

    for (i = 0; i < 2; i++) {
	printf("sfmt[N][%d] = %016"PRIx64"\n", i, sfmt->status[N][i]);
	work = sfmt->status[N][i] & parity[i];
	for (j = 0; j < 52; j++) {
	    inner ^= work & 1;
	    work = work >> 1;
	}
    }
    /* check OK */
    if (inner == 1) {
	printf("DEBUG:success in parity check\n");
	return;
    }
    /* check NG, and modification */
    printf("DEBUG:failure in parity check\n");
    for (i = 0; i < 2; i++) {
	work = 1;
	for (j = 0; j < 52; j++) {
	    if ((work & parity[i]) != 0) {
		printf("DEBUG: before = %"PRIx64"\n", sfmt->status[N][i]);
		sfmt->status[N][i] ^= work;
		printf("DEBUG: change %dth bit of index %d\n", j, i);
		printf("DEBUG: work = %"PRIx64", parity[%d] = %"PRIx64"\n",
		       work, i, parity[i]);
		printf("DEBUG: after = %"PRIx64"\n", sfmt->status[N][i]);
		return;
	    }
	    work = work << 1;
	}
    }
}
#endif

static void make_zero_state(dsfmt_t *sfmt, GF2X& poly) {
    dsfmt_t sfmtnew;
    uint64_t ar[2];
    int i;

    memset(&sfmtnew, 0, sizeof(sfmtnew));
    for (i = 0; i <= deg(poly); i++) {
	if (coeff(poly, i) != 0) {
	    add_rnd(&sfmtnew, sfmt);
	}
	gen_rand104sp(sfmt, ar, 0);
    }
    *sfmt = sfmtnew;
}