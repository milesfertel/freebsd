/*-
 * Copyright (C) 2004 Maxim Sobolev <sobomax@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Test for mergesort() routine.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: head/lib/libc/tests/stdlib/mergesort_test.c 290538 2015-11-08 07:03:17Z ngie $");

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "test-sort.h"

#define ELT_MAX 16
/*
 * Number of keys in stability test.
 * Chosen to be low enough that every element would probabilistically need
 * to be properly ordered for stability, without being trivially small.
 */
#define KEYS 50
/*
 * Definitions for sizeable_mergesort_test
 * Test from 2^BASE_EXP to 2^MAX_EXP elements of type SORT_TYPE
 */
#define BASE_EXP 10
#define MAX_EXP  18
#define SORT_TYPE int

// Number of chars in the odd elt subarray
#define NCHARS 5

// Canary value to verify buffer is intact
#define CANARY 0XDEADBEEF

#define check_array_eq(it, len, expected, actual) do { \
    for (it = 0; it < len; it++) \
        ATF_CHECK_MSG( \
            actual[it] == expected[it], \
            "item at index %d didn't match: %d != %d", \
            it, actual[it], expected[it]); \
} while(0)

/*
 * Structures to allow for the creation of predefined variable sized arrays.
 * The len field is necesary to determine how many elements are being used
 * in the elts array
 */
struct int_array {
	int len;
	int elts[ELT_MAX];
};

struct char_array {
	int len;
	char elts[ELT_MAX];
};

/*
 * Sorting arrays of small size and simple element combinations to ensure
 * basic competency.
 */
ATF_TC_WITHOUT_HEAD(trivial_mergesort_test);
ATF_TC_BODY(trivial_mergesort_test, tc)
{
	static const struct int_array testarrays[] = {
	    {1, {0}},       {2, {0, 0}},    {3, {0, 0, 0}},   {2, {0, 1}},
	    {2, {1, 0}},    {3, {0, 1, 2}}, {3, {0, 2, 1}},   {3, {1, 0, 2}},
	    {3, {1, 2, 0}}, {3, {2, 0, 1}}, {3, {2, 1, 0}},   {3, {0, 1, 1}},
	    {3, {1, 0, 1}}, {3, {1, 1, 0}}, {3, {0, -1, -1}}, {3, {0, -1, 0}}};

	int sresvector[ELT_MAX];
	int testvector[ELT_MAX];
	int num_tests = nitems(testarrays);
	int i, j, len;
	size_t size;

	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		size = len * sizeof(testvector[0]);

		/* Populate test vectors */
		memcpy(testvector, testarrays[i].elts, size);
		memcpy(sresvector, testarrays[i].elts, size);

		/* Sort using mergesort(3) */
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, len, sizeof(sresvector[0]), sorthelp);

		check_array_eq(j, len, testvector, sresvector);
	}
}

/*
 * Sorting arrays in which the elements are either ordered, partially ordered,
 * or reverse ordered.
 */
ATF_TC_WITHOUT_HEAD(ordering_mergesort_test);
ATF_TC_BODY(ordering_mergesort_test, tc)
{
	static const struct int_array testarrays[] = {
	    {3, {-1, -2, -3}},
	    {3, {-3, -2, -1}},
	    {11, {1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1}},
	    {10, {5, 5, 4, 4, 3, 3, 2, 2, 1, 1}},
	    {10, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
	    {10, {1, 2, 3, 4, 5, 6, -7, -8, -9, -10}},
	    {10, {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}}};

	int sresvector[ELT_MAX];
	int testvector[ELT_MAX];
	int num_tests = nitems(testarrays);
	int i, j, len;
	size_t size;

	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		size = len * sizeof(testvector[0]);

		/* Populate test vectors */
		memcpy(testvector, testarrays[i].elts, size);
		memcpy(sresvector, testarrays[i].elts, size);

		/* Sort using mergesort(3) */
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, len, sizeof(sresvector[0]), sorthelp);

		check_array_eq(j, len, testvector, sresvector);
	}
}

/*
 * Sorting arrays with a variety of challenges: i.e. max integer values,
 * multiple identical values, etc.
 */
ATF_TC_WITHOUT_HEAD(misc_mergesort_test);
ATF_TC_BODY(misc_mergesort_test, tc)
{
	static const struct int_array testarrays[] = {
	    {10, {42, 9, 17, 123, 602, -3, 123, 969, -11, INT_MAX}},
	    {10, {-11, -3, 9, 17, 42, 54, 54, 602, 969, INT_MIN}}};

	int sresvector[ELT_MAX];
	int testvector[ELT_MAX];
	int num_tests = nitems(testarrays);
	int i, j, len;
	size_t size;

	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		size = len * sizeof(testvector[0]);

		/* Populate test vectors */
		memcpy(testvector, testarrays[i].elts, size);
		memcpy(sresvector, testarrays[i].elts, size);

		/* Sort using mergesort(3) */
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, len, sizeof(sresvector[0]), sorthelp);

		check_array_eq(j, len, testvector, sresvector);
	}
}

/*
 * Sort an array of random integers.
 */
ATF_TC_WITHOUT_HEAD(rand_mergesort_test);
ATF_TC_BODY(rand_mergesort_test, tc)
{
	int sresvector[IVEC_LEN];
	int testvector[IVEC_LEN];
	int i, j;
	size_t size;

	for (j = 2; j < IVEC_LEN; j++) {
		size = j * sizeof(testvector[0]);
		/* Populate test vectors */
		memcpy(testvector, initvector, size);
		memcpy(sresvector, initvector, size);

		/* Sort using mergesort(3) */
		mergesort(testvector, j, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, j, sizeof(sresvector[0]), sorthelp);

		/* Compare results */
		check_array_eq(i, j, testvector, sresvector);
	}
}

/*
 * Sort an array of chars
 */
ATF_TC_WITHOUT_HEAD(small_elt_mergesort_test);
ATF_TC_BODY(small_elt_mergesort_test, tc)
{
	static const struct char_array testarrays[] = {
	    {1, {0}},
	    {2, {0, 0}},
	    {3, {0, 0, 0}},
	    {2, {0, 1}},
	    {2, {1, 0}},
	    {3, {0, 1, 2}},
	    {3, {0, 2, 1}},
	    {3, {1, 0, 2}},
	    {3, {1, 2, 0}},
	    {3, {2, 0, 1}},
	    {3, {2, 1, 0}},
	    {3, {0, 1, 1}},
	    {3, {1, 0, 1}},
	    {3, {1, 1, 0}},
	    {3, {0, -1, -1}},
	    {3, {0, -1, 0}},
	    {3, {-1, -2, -3}},
	    {3, {-3, -2, -1}},
	    {11, {1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1}},
	    {10, {5, 5, 4, 4, 3, 3, 2, 2, 1, 1}},
	    {10, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
	    {10, {1, 2, 3, 4, 5, 6, -7, -8, -9, -10}},
	    {10, {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}},
	    {10, {42, 9, 17, 123, 62, -3, 123, 99, -11, CHAR_MAX}},
	    {10, {-11, -3, 9, 17, 42, 54, 54, -62, 9, CHAR_MIN}}};

	char sresvector[ELT_MAX];
	char testvector[ELT_MAX];
	int num_tests = nitems(testarrays);
	int i, j, len, ret;
	size_t size;
	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		size = len * sizeof(testvector[0]);
		/* Populate test vectors */
		memcpy(testvector, testarrays[i].elts, size);
		memcpy(sresvector, testarrays[i].elts, size);

		atf_tc_expect_fail(
		    "Mergesort implementation does not support arguments less "
		    "than sizeof(void *)/2");
		/* Sort using mergesort(3) */
		ret = mergesort(testvector, len, sizeof(testvector[0]),
		    csorthelp);
		ATF_CHECK(ret == 0);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, len, sizeof(sresvector[0]),
		    csorthelp);

		/* Compare results */
		check_array_eq(j, len, testvector, sresvector);
	}
}

/*
 * Sort an array of char[5] arrays. Tests for possible alignment issues.
 */
ATF_TC_WITHOUT_HEAD(oddelt_mergesort_test);
ATF_TC_BODY(oddelt_mergesort_test, tc)
{
	struct odd_array {
		int len;
		char elts[ELT_MAX][NCHARS];
	};

	static const struct odd_array testarrays[] = {
	    {2, {{0, 1, 2, 3, 4}, {1, 2, 3, 4, 5}}},
	    {3, {{1, 2, 3, 4, 5}, {0, 1, 2, 3, 4}, {2, 3, 1, 0, 0}}},
	    {3, {{-1, 2, -3, 4, 5}, {0, 1, 2, -3, 4}, {2, 3, -1, 0, 0}}},
	    {3, {{-1, 7, 4, 0, 0}, {0, 1, 2, 0, 0}, {4, 3, 5, 0, 0}}},
	    {3, {{1, 7, 4, 0, 0}, {0, 1, -2, 0, 0}, {4, -3, 5, 0, 0}}},
	    {10,
	     {{-1, 0, 0, 0, 0},
	      {-2, 0, 0, 0, 0},
	      {-3, 0, 0, 0, 0},
	      {-4, 0, 0, 0, 0},
	      {-5, 0, 0, 0, 0},
	      {-6, 0, 0, 0, 0},
	      {-7, 0, 0, 0, 0},
	      {-8, 0, 0, 0, 0},
	      {-9, 0, 0, 0, 0},
	      {-10, 0, 0, 0, 0}}}};

	char sresvector[ELT_MAX][NCHARS];
	char testvector[ELT_MAX][NCHARS];
	int num_tests = nitems(testarrays);
	int i, j, k, len;

	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		/* Populate test vectors */
		for (j = 0; j < len; j++) {
			for (k = 0; k < NCHARS; k++)
				testvector[j][k] = sresvector[j][k] =
				    testarrays[i].elts[j][k];
		}
		/* Sort using mergesort(3) */
		mergesort(testvector, len, sizeof(testvector[0]), oddsorthelp);
		/* Sort using reference slow sorting routine */
		insertionsort(sresvector, len, sizeof(sresvector[0]),
		    oddsorthelp);
		for (j = 0; j < len; j++)
			check_array_eq(k, NCHARS, testvector[j], sresvector[j]);
	}
}

/*
 * Sort an array of int*'s
 */
ATF_TC_WITHOUT_HEAD(pointer_mergesort_test);
ATF_TC_BODY(pointer_mergesort_test, tc)
{
	struct ptr_array {
		int len;
		int *elts[ELT_MAX];
	};

	static const struct ptr_array testarrays[] = {
	    {3, {NULL, (int *)0x200, (int *)0x100}},
	    {3, {(int *)0xFFFF, (int *)0xFFFE, (int *)0xFFFD}},
	    {3, {NULL, NULL, NULL}},
	    {6,
	     {(int *)0xFFFFFFFF, (int *)0x100000000, NULL, (int *)1234,
	      (int *)0x1234, (int *)0xFFFFFFFF}}};

	int *sresvector[ELT_MAX];
	int *testvector[ELT_MAX];
	int num_tests = nitems(testarrays);
	int i, j, len;
	for (i = 0; i < num_tests; i++) {
		len = testarrays[i].len;
		/* Populate test vectors */
		for (j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using mergesort(3) */
		mergesort(testvector, len, sizeof(testvector[0]), ptrsorthelp);
		/* Reference sort */
		insertionsort(sresvector, len, sizeof(testvector[0]),
		    ptrsorthelp);

		for (j = 0; j < len; j++)
			ATF_CHECK_MSG(testvector[j] == sresvector[j],
			    "item at index %d didn't match: %p != %p\n",
			    j, testvector[j], sresvector[j]);
	}
}

/*
 * Sort an array of random int*'s
 */
ATF_TC_WITHOUT_HEAD(rand_pointer_mergesort_test);
ATF_TC_BODY(rand_pointer_mergesort_test, tc)
{
	int *rand_sresvector[IVEC_LEN];
	int *rand_testvector[IVEC_LEN];
	int i, j;

	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++)
			rand_testvector[i] = rand_sresvector[i] =
			    &(initvector[arc4random() % IVEC_LEN]);

		/* Sort using mergesort(3) */
		mergesort(rand_testvector, j, sizeof(rand_testvector[0]),
		    ptrsorthelp);
		/* Reference sort */
		insertionsort(rand_sresvector, j, sizeof(rand_sresvector[0]),
		    ptrsorthelp);

		/* Compare results */
		for (i = 0; i < j; i++)
			ATF_CHECK_MSG(rand_testvector[i] == rand_sresvector[i],
			    "item at index %d didn't match: %p != %p\n",
			    i, rand_testvector[i], rand_sresvector[i]);
	}
}

/*
 * Sort an array of nontrivially large structs
 */
ATF_TC_WITHOUT_HEAD(bigstruct_mergesort_test);
ATF_TC_BODY(bigstruct_mergesort_test, tc)
{
	struct big sresvector[IVEC_LEN];
	struct big testvector[IVEC_LEN];
	int i, j;
	size_t size = SPACE_LEN * sizeof(int);

	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++) {
			testvector[i].value = sresvector[i].value =
			    initvector[i];
			memset(testvector[i].spacetaker, CANARY, size);
			memset(sresvector[i].spacetaker, CANARY, size);
		}

		/* Sort using mergesort(3) */
		mergesort(testvector, j, sizeof(testvector[0]), bigsorthelp);
		/* Reference sort */
		insertionsort(sresvector, j, sizeof(sresvector[0]),
		    bigsorthelp);

		/* Compare results */
		for (i = 0; i < j; i++) {
			ATF_CHECK_MSG(testvector[i].value ==
			    sresvector[i].value,
			    "item at index %d didn't match: %d != %d\n",
			    i, testvector[i].value, sresvector[i].value);
			ATF_CHECK_MSG(memcmp(testvector[i].spacetaker,
			    sresvector[i].spacetaker, size) == 0,
			    "Spacetaker corrupted at index %d\n", i);
		}
	}
}

/*
 * Sort an array of structs with identical values but different keys in order
 * to check for stability. The sorted elements with the same value should
 * be in the same order by key.
 */
ATF_TC_WITHOUT_HEAD(stability_mergesort_test);
ATF_TC_BODY(stability_mergesort_test, tc)
{
	struct stable sresvector[IVEC_LEN];
	struct stable testvector[IVEC_LEN];
	int i, j;
	int keys[KEYS] = {0};
	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++) {
			int value = arc4random() % KEYS;
			testvector[i].value = sresvector[i].value = value;
			testvector[i].key = sresvector[i].key = keys[value];
			keys[value]++;
		}

		/* Sort using mergesort(3) */
		mergesort(testvector, j, sizeof(testvector[0]), stablesorthelp);
		/* Reference sort */
		insertionsort(sresvector, j, sizeof(sresvector[0]),
		    stablesorthelp);

		/* Compare results */
		for (i = 0; i < j; i++) {
			ATF_CHECK_MSG(testvector[i].value ==
			    sresvector[i].value &&
			    testvector[i].key == sresvector[i].key,
			    "item at index %d didn't match: value %d "
			    "!= %d or key %d != %d\n",
			    i, testvector[i].value, sresvector[i].value,
			    testvector[i].key, sresvector[i].key);
		}
	}
}

static void
rand_test(int elts)
{
	int i;
	size_t size = sizeof(SORT_TYPE) * elts;
	SORT_TYPE *testvector = malloc(size);
	SORT_TYPE *sresvector = malloc(size);

	arc4random_buf(testvector, size);
	memcpy(sresvector, testvector, size);

	/* Sort using mergesort(3) */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	insertionsort(sresvector, elts, sizeof(SORT_TYPE), sorthelp);

	check_array_eq(i, elts, testvector, sresvector);

	free(testvector);
	free(sresvector);
}

static void
sort_test(int elts)
{
	int i;
	size_t size = sizeof(SORT_TYPE) * elts;
	SORT_TYPE *testvector = malloc(size);
	SORT_TYPE *sresvector = malloc(size);

	for (i = 0; i < elts; i++) {
		testvector[i] = sresvector[i] = i;
	}

	/* Sort using mergesort(3) */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	insertionsort(sresvector, elts, sizeof(SORT_TYPE), sorthelp);

	check_array_eq(i, elts, testvector, sresvector);

	free(testvector);
	free(sresvector);
}

static void
partial_test(int elts)
{
	int i;
	size_t size = sizeof(SORT_TYPE) * elts;
	SORT_TYPE *testvector = malloc(size);
	SORT_TYPE *sresvector = malloc(size);

	for (i = 0; i < elts; i++) {
		if (i <= elts / 2)
			testvector[i] = sresvector[i] = i;
		else
			testvector[i] = sresvector[i] = arc4random();
	}

	/* Sort using mergesort(3) */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	insertionsort(sresvector, elts, sizeof(SORT_TYPE), sorthelp);

	check_array_eq(i, elts, testvector, sresvector);

	free(testvector);
	free(sresvector);
}

static void
reverse_test(int elts)
{
	int i;
	size_t size = sizeof(SORT_TYPE) * elts;
	SORT_TYPE *testvector = malloc(size);
	SORT_TYPE *sresvector = malloc(size);

	for (i = 0; i < elts; i++) {
		testvector[i] = sresvector[i] = elts - i;
	}

	/* Sort using mergesort(3) */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	insertionsort(sresvector, elts, sizeof(SORT_TYPE), sorthelp);

	check_array_eq(i, elts, testvector, sresvector);

	free(testvector);
	free(sresvector);
}

/*
 * Sort an array of integers from 2^BASE_EXP to 2^MAX_EXP elements
 */
ATF_TC_WITHOUT_HEAD(sizeable_mergesort_test);
ATF_TC_BODY(sizeable_mergesort_test, tc)
{
	int max = pow(2, MAX_EXP);
	int base = pow(2, BASE_EXP);
	for (int elts = base; elts < max; elts *= 2) {
		rand_test(elts);
		sort_test(elts);
		partial_test(elts);
		reverse_test(elts);
	}
}


/*
 * Test cases to ensure correctness of stdlib mergesort function
 */
ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, trivial_mergesort_test);
	ATF_TP_ADD_TC(tp, ordering_mergesort_test);
	ATF_TP_ADD_TC(tp, misc_mergesort_test);
	ATF_TP_ADD_TC(tp, rand_mergesort_test);
	ATF_TP_ADD_TC(tp, small_elt_mergesort_test);
	ATF_TP_ADD_TC(tp, oddelt_mergesort_test);
	ATF_TP_ADD_TC(tp, pointer_mergesort_test);
	ATF_TP_ADD_TC(tp, rand_pointer_mergesort_test);
	ATF_TP_ADD_TC(tp, bigstruct_mergesort_test);
	ATF_TP_ADD_TC(tp, stability_mergesort_test);
	ATF_TP_ADD_TC(tp, sizeable_mergesort_test);

	return (atf_no_error());
}
