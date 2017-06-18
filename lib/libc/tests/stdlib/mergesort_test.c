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
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
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
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
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
		mergesort(testvector, len, sizeof(testvector[0]), sorthelp);
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
		mergesort(testvector, j, sizeof(testvector[0]), sorthelp);
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
		atf_tc_expect_fail(
		    "Mergesort implementation does not support arguments less "
		    "than sizeof(void *)/2");
		ATF_CHECK(
		    mergesort(
			testvector, len, sizeof(testvector[0]), sorthelp) == 0);
		/* Sort using reference slow sorting routine */
		scsort(sresvector, len);

		for (int k = 0; k < len; k++)
			ATF_CHECK_MSG(testvector[k] == sresvector[k],
			    "item at index %d didn't match: %d != %d", k,
			    testvector[k], sresvector[k]);
	}
}

ATF_TP_ADD_TCS(tp) {
	ATF_TP_ADD_TC(tp, trivial_mergesort_test);
	ATF_TP_ADD_TC(tp, ordering_mergesort_test);
	ATF_TP_ADD_TC(tp, misc_mergesort_test);
	ATF_TP_ADD_TC(tp, rand_mergesort_test);
	/* This will fail until new mergesort is completed */
	ATF_TP_ADD_TC(tp, small_elt_mergesort_test);

	return (atf_no_error());
}
