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
#include <stdio.h>
#include <stdlib.h>

#include "test-sort.h"
#include "../../stdlib/wiki.c"
#define ELT_MAX 16

struct int_array {
	int len;
	int elts[ELT_MAX];
};

struct char_array {
	int len;
	char elts[ELT_MAX];
};

ATF_TC_WITHOUT_HEAD(trivial_mergesort_test);
ATF_TC_BODY(trivial_mergesort_test, tc) {
	struct int_array testarrays[] = {
	    {1, {0}},       {2, {0, 0}},    {3, {0, 0, 0}},   {2, {0, 1}},
	    {2, {1, 0}},    {3, {0, 1, 2}}, {3, {0, 2, 1}},   {3, {1, 0, 2}},
	    {3, {1, 2, 0}}, {3, {2, 0, 1}}, {3, {2, 1, 0}},   {3, {0, 1, 1}},
	    {3, {1, 0, 1}}, {3, {1, 1, 0}}, {3, {0, -1, -1}}, {3, {0, -1, 0}}};

	int sresvector[ELT_MAX];
	int testvector[ELT_MAX];
	int num_tests = nitems(testarrays);

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using mergesort(3) */
		WikiSort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		ssort(sresvector, len);

		for (int k = 0; k < len; k++)
			ATF_CHECK_MSG(
			    testvector[k] == sresvector[k],
			    "item at index %d didn't match: %d != %d", k,
			    testvector[k], sresvector[k]);
	}
}

ATF_TC_WITHOUT_HEAD(ordering_mergesort_test);
ATF_TC_BODY(ordering_mergesort_test, tc) {
	struct int_array testarrays[] = {
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

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using mergesort(3) */
		WikiSort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		ssort(sresvector, len);

		for (int k = 0; k < len; k++)
			ATF_CHECK_MSG(
			    testvector[k] == sresvector[k],
			    "item at index %d didn't match: %d != %d", k,
			    testvector[k], sresvector[k]);
	}
}

ATF_TC_WITHOUT_HEAD(misc_mergesort_test);
ATF_TC_BODY(misc_mergesort_test, tc) {
	struct int_array testarrays[] = {
	    {10, {42, 9, 17, 123, 602, -3, 123, 969, -11, INT_MAX}},
	    {10, {-11, -3, 9, 17, 42, 54, 54, 602, 969, INT_MIN}}};

	int sresvector[ELT_MAX];
	int testvector[ELT_MAX];
	int num_tests = nitems(testarrays);

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using mergesort(3) */
		WikiSort(testvector, len, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		ssort(sresvector, len);

		for (int k = 0; k < len; k++)
			ATF_CHECK_MSG(
			    testvector[k] == sresvector[k],
			    "item at index %d didn't match: %d != %d", k,
			    testvector[k], sresvector[k]);
	}
}

ATF_TC_WITHOUT_HEAD(rand_mergesort_test);
ATF_TC_BODY(rand_mergesort_test, tc) {
	int sresvector[IVEC_LEN];
	int testvector[IVEC_LEN];
	int i, j;

	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++)
			testvector[i] = sresvector[i] = initvector[i];

		/* Sort using mergesort(3) */
		WikiSort(testvector, j, sizeof(testvector[0]), sorthelp);
		/* Sort using reference slow sorting routine */
		ssort(sresvector, j);

		/* Compare results */
		for (i = 0; i < j; i++)
			ATF_CHECK_MSG(
			    testvector[i] == sresvector[i],
			    "item at index %d didn't match: %d != %d", i,
			    testvector[i], sresvector[i]);
	}
}

ATF_TC_WITHOUT_HEAD(small_elt_mergesort_test);
ATF_TC_BODY(small_elt_mergesort_test, tc) {
	struct char_array testarrays[] = {
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

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using mergesort(3) */
		ATF_CHECK(
		    WikiSort(
			testvector, len, sizeof(testvector[0]), csorthelp) == 0);
		/* Sort using reference slow sorting routine */
		scsort(sresvector, len);

		for (int k = 0; k < len; k++)
			ATF_CHECK_MSG(
			    testvector[k] == sresvector[k],
			    "item at index %d didn't match: %d != %d", k,
			    testvector[k], sresvector[k]);
	}
}

ATF_TC_WITHOUT_HEAD(oddelt_WikiSort_test);
ATF_TC_BODY(oddelt_WikiSort_test, tc) {
	#define NCHARS 5
	struct odd_array {
		int len;
		char elts[ELT_MAX][NCHARS];
	};

	struct odd_array testarrays[] = {
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

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++) {
			for (int k = 0; k < NCHARS; k++)
				testvector[j][k] = sresvector[j][k] =
				    testarrays[i].elts[j][k];
		}
		/* Sort using WikiSort(3) */
		WikiSort(testvector, len, sizeof(testvector[0]), oddsorthelp);
		/* Sort using reference slow sorting routine */
		mergesort(sresvector, len, sizeof(sresvector[0]), oddsorthelp);
		for (int k = 0; k < len; k++)
			for (int l = 0; l < NCHARS; l++) {
				if (testvector[k][l] != sresvector[k][l])
					printf(
					    "item at index %d didn't match: %d "
					    "!= %d\n",
					    k, testvector[k][l],
					    sresvector[k][l]);
			}
	}
}

ATF_TC_WITHOUT_HEAD(pointer_WikiSort_test);
ATF_TC_BODY(pointer_WikiSort_test, tc) {
	struct ptr_array {
		int len;
		int *elts[ELT_MAX];
	};

	struct ptr_array testarrays[] = {
	    {3, {NULL, (int *)0x200, (int *)0x100}},
	    {3, {(int *)0xFFFF, (int *)0xFFFE, (int *)0xFFFD}},
	    {3, {NULL, NULL, NULL}},
	    {6,
	     {(int *)0xFFFFFFFF, (int *)0x100000000, NULL, (int *)1234,
	      (int *)0x1234, (int *)0xFFFFFFFF}}};

	int *sresvector[ELT_MAX];
	int *testvector[ELT_MAX];
	int num_tests = nitems(testarrays);

	for (int i = 0; i < num_tests; i++) {
		int len = testarrays[i].len;
		/* Populate test vectors */
		for (int j = 0; j < len; j++)
			testvector[j] = sresvector[j] = testarrays[i].elts[j];
		/* Sort using WikiSort(3) */
		WikiSort(testvector, len, sizeof(testvector[0]), ptrsorthelp);
		/* Reference sort */
		mergesort(sresvector, len, sizeof(testvector[0]), ptrsorthelp);

		for (int k = 0; k < len; k++)
			if (testvector[k] != sresvector[k])
				printf(
				    "item at index %d didn't match: %p != %p\n",
				    k, testvector[k], sresvector[k]);
	}

	int *rand_sresvector[IVEC_LEN];
	int *rand_testvector[IVEC_LEN];
	int i, j;

	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++)
			rand_testvector[i] = rand_sresvector[i] =
			    &(initvector[rand() % IVEC_LEN]);

		/* Sort using WikiSort(3) */
		WikiSort(rand_testvector, j, sizeof(rand_testvector[0]),
			 ptrsorthelp);
		/* Reference sort */
		mergesort(rand_sresvector, j, sizeof(rand_sresvector[0]),
			  ptrsorthelp);

		/* Compare results */
		for (i = 0; i < j; i++)
			if (rand_testvector[i] != rand_sresvector[i])
				printf(
				    "item at index %d didn't match: %p != %p\n",
				    i, rand_testvector[i], rand_sresvector[i]);
	}
}

ATF_TC_WITHOUT_HEAD(bigstruct_WikiSort_test);
ATF_TC_BODY(bigstruct_WikiSort_test, tc) {
	struct big sresvector[IVEC_LEN];
	struct big testvector[IVEC_LEN];
	int i, j;

	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++)
			testvector[i].value = sresvector[i].value =
			    initvector[i];

		/* Sort using WikiSort(3) */
		WikiSort(testvector, j, sizeof(testvector[0]), bigsorthelp);
		/* Reference sort */
		mergesort(sresvector, j, sizeof(testvector[0]), bigsorthelp);

		/* Compare results */
		for (i = 0; i < j; i++)
			if (testvector[i].value != sresvector[i].value)
				printf(
				    "item at index %d didn't match: %d != %d\n",
				    i, testvector[i].value,
				    sresvector[i].value);
	}
}

ATF_TC_WITHOUT_HEAD(stability_WikiSort_test);
ATF_TC_BODY(stability_WikiSort_test, tc) {
	struct stable sresvector[IVEC_LEN];
	struct stable testvector[IVEC_LEN];
	int i, j;
	int keys[50] = {0};
	for (j = 2; j < IVEC_LEN; j++) {
		/* Populate test vectors */
		for (i = 0; i < j; i++){
			int value = rand() % 50;
			testvector[i].value = sresvector[i].value = value;
			testvector[i].key = sresvector[i].key = keys[value];
			keys[value]++;
		}

		/* Sort using WikiSort(3) */
		WikiSort(testvector, j, sizeof(testvector[0]), stablesorthelp);
		/* Reference sort */
		mergesort(sresvector, j, sizeof(testvector[0]), stablesorthelp);

		/* Compare results */
		for (i = 0; i < j; i++){
			if (testvector[i].value != sresvector[i].value || testvector[i].key != sresvector[i].key)
				printf(
				    "item at index %d didn't match: value %d != %d or key %d != %d\n",
				    i, testvector[i].value, sresvector[i].value, testvector[i].key, sresvector[i].key);
		}
	}
}

#define SORT_TYPE int
void check(SORT_TYPE *array1, SORT_TYPE *array2, int elts) {
	for (int k = 0; k < elts; k++)
		ATF_CHECK_MSG(
		    array1[k] == array2[k],
		    "item at index %d didn't match: %d != %d", k,
		    array1[k], array2[k]);
}
void rand_test(int elts) {
	SORT_TYPE *testvector = malloc(sizeof(SORT_TYPE) * elts);
	SORT_TYPE *sresvector = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		testvector[i] = sresvector[i] = rand();
	}
	/* Sort using mergesort(3) */
	WikiSort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	check(testvector, sresvector, elts);
	free(testvector);
	free(sresvector);
}

void sort_test(int elts) {
	SORT_TYPE *testvector = malloc(sizeof(SORT_TYPE) * elts);
	SORT_TYPE *sresvector = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		testvector[i] = sresvector[i] = i;
	}
	/* Sort using mergesort(3) */
	WikiSort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	check(testvector, sresvector, elts);
	free(testvector);
	free(sresvector);
}

void partial_test(int elts) {
	SORT_TYPE *testvector = malloc(sizeof(SORT_TYPE) * elts);
	SORT_TYPE *sresvector = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		if (i <= elts / 2)
			testvector[i] = sresvector[i] = i;
		else
			testvector[i] = sresvector[i] = rand();
	}
	/* Sort using mergesort(3) */
	WikiSort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	check(testvector, sresvector, elts);
	free(testvector);
	free(sresvector);
}

void reverse_test(int elts) {
	SORT_TYPE *testvector = malloc(sizeof(SORT_TYPE) * elts);
	SORT_TYPE *sresvector = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		testvector[i] = sresvector[i] = elts - i;
	}
	/* Sort using mergesort(3) */
	WikiSort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	/* Sort using reference sorting routine */
	mergesort(testvector, elts, sizeof(SORT_TYPE), sorthelp);
	check(testvector, sresvector, elts);
	free(testvector);
	free(sresvector);
}

ATF_TC_WITHOUT_HEAD(sizeable_WikiSort_test);
ATF_TC_BODY(sizeable_WikiSort_test, tc) {
	int max = pow(2, 30);
	for (int elts = 1024; elts < max; elts *= 2) {
		rand_test(elts);
		sort_test(elts);
		partial_test(elts);
		reverse_test(elts);
	}
}

ATF_TP_ADD_TCS(tp) {
	ATF_TP_ADD_TC(tp, trivial_mergesort_test);
	ATF_TP_ADD_TC(tp, ordering_mergesort_test);
	ATF_TP_ADD_TC(tp, misc_mergesort_test);
	ATF_TP_ADD_TC(tp, rand_mergesort_test);

	/* This will fail until new mergesort is completed */
	ATF_TP_ADD_TC(tp, small_elt_mergesort_test);

	ATF_TP_ADD_TC(tp, oddelt_WikiSort_test);
	ATF_TP_ADD_TC(tp, pointer_WikiSort_test);
	ATF_TP_ADD_TC(tp, bigstruct_WikiSort_test);
	ATF_TP_ADD_TC(tp, stability_WikiSort_test);
	ATF_TP_ADD_TC(tp, sizeable_WikiSort_test);

	return (atf_no_error());
}
