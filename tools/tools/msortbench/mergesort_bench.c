#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#ifdef WIKI
#include "stdlib/wiki.c"
#endif

int
sorthelp(const void *a, const void *b)
{
	if (*(int *)a > *(int *)b)
		return 1;
	if (*(int *)a < *(int *)b)
		return -1;
	return 0;
}

#define NARGS 5

enum test { RAND, SORT, PART, REV, INVALID_TEST };
enum sort { MERGE, WIKI, QUICK, HEAP, INVALID_ALG };

static void
sort(int *testarray, int elts, enum sort s)
{
	switch (s) {
	case MERGE:
		mergesort(testarray, (size_t)elts, sizeof(int), sorthelp);
		break;
#ifdef WIKI
	case WIKI:
		WikiSort(testarray, (size_t)elts, sizeof(int), sorthelp);
		break;
#endif
	case QUICK:
		qsort(testarray, (size_t)elts, sizeof(int), sorthelp);
		break;
	case HEAP:
		heapsort(testarray, (size_t)elts, sizeof(int), sorthelp);
		break;
	}
}

static void
rand_bench(int elts, enum sort s)
{
	size_t size = sizeof(int) * elts;
	int *array = malloc(size);
	arc4random_buf(array, size);
	sort(array, elts, s);
	free(array);
}

static void
sort_bench(int elts, enum sort s)
{
	size_t size = sizeof(int) * elts;
	int *array = malloc(size);
	for (int i = 0; i < elts; i++) {
		array[i] = i;
	}
	sort(array, elts, s);
	free(array);
}

static void
partial_bench(int elts, enum sort s)
{
	size_t size = sizeof(int) * elts;
	int *array = malloc(size);
	for (int i = 0; i < elts; i++) {
		if (i <= elts / 2)
			array[i] = i;
		else
			array[i] = rand();
	}
	sort(array, elts, s);
	free(array);
}

static void
reverse_bench(int elts, enum sort s)
{
	size_t size = sizeof(int) * elts;
	int *array = malloc(size);
	for (int i = 0; i < elts; i++) {
		array[i] = elts - i;
	}
	sort(array, elts, s);
	free(array);
}

static void
run_bench(enum sort s, enum test t, int runs, int elts)
{
	for (int i = 0; i < runs; i++) {
		switch (t) {
		case RAND:
			rand_bench(elts, s);
			break;
		case SORT:
			sort_bench(elts, s);
			break;
		case PART:
			partial_bench(elts, s);
			break;
		case REV:
			reverse_bench(elts, s);
			break;
		}
	}
}

static enum sort
parse_alg(char *alg)
{
	if (strcmp(alg, "merge") == 0)
		return MERGE;
#ifdef WIKI
	else if (strcmp(alg, "wiki") == 0)
		return WIKI;
#endif
	else if (strcmp(alg, "quick") == 0)
		return QUICK;
	else if (strcmp(alg, "heap") == 0)
		return HEAP;
	else
		return INVALID_ALG;
}

static enum test
parse_test(char *test)
{
	if (strcmp(test, "rand") == 0)
		return RAND;
	else if (strcmp(test, "sort") == 0)
		return SORT;
	else if (strcmp(test, "part") == 0)
		return PART;
	else if (strcmp(test, "rev") == 0)
		return REV;
	else
		return INVALID_TEST;
}

static void
usage(const char *progname)
{
	printf("Usage:\n");
	printf("\t%s: [alg] [test] [runs] [elt_power]\n", progname);
	printf("\n");
	printf("Valid algs:\n");
#ifdef WIKI
	printf("\theap merge quick wiki\n");
#else
	printf("\theap merge quick\n");
#endif
	printf("Valid tests:\n");
	printf("\trand sort part rev\n");
	printf("\trand: Random element array \n");
	printf("\tsort: Increasing order array \n");
	printf("\tpart: Partially ordered array\n");
	printf("\trev: Decreasing order array\n");
	printf("Run the algorithm [runs] times with 2^[elt_power] elements\n");
}

/*
 * Runs a sorting algorithm with a provided data configuration according to
 * command line arguments
 */
int
main(int argc, char *argv[])
{
	const char *progname = argv[0];
	int runs, elts;
	if (argc != NARGS) {
		usage(progname);
		exit(EX_USAGE);
	}

	enum sort s = parse_alg(argv[1]);
	if (s == INVALID_ALG) {
		usage(progname);
		exit(EX_USAGE);
	}
	enum test t = parse_test(argv[2]);
	if (t == INVALID_TEST) {
		usage(progname);
		exit(EX_USAGE);
	}
	runs = atoi(argv[3]);
	elts = pow(2, atoi(argv[4]));

	run_bench(s, t, runs, elts);
}
