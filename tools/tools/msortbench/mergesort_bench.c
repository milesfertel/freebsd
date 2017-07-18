#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stdlib/wiki.c"

int Trials;

int
sorthelp(const void *a, const void *b)
{
	if (*(int *)a > *(int *)b)
		return 1;
	if (*(int *)a < *(int *)b)
		return -1;
	return 0;
}

#define SORT_CMP  	sorthelp
#define SORT_TYPE 	int
#define TESTS     	4
/*
 * Average the times over multiple trials, the number of trials decreasing
 * as the number of elements increases
 */
#define TRIALS    	30
#define MIN_TRIALS	5

// Test from 2^BASE to 2^MAX elements
#define BASE      	10
#define MAX       	27

enum test { RAND, SORT, PART, REV };

static double
time_mergesort(int elts, SORT_TYPE *array)
{
	SORT_TYPE *testarray = malloc(sizeof(SORT_TYPE) * elts);
	memcpy(testarray, array, elts * sizeof(SORT_TYPE));
	clock_t sum = 0;
	for (int i = 0; i < Trials; i++) {
		clock_t begin = clock();
		mergesort(testarray, (size_t)elts, sizeof(SORT_TYPE), SORT_CMP);
		clock_t end = clock();
		sum += (end - begin);
	}
	clock_t average = sum / (clock_t)Trials;
	free(testarray);
	return (double)average / CLOCKS_PER_SEC;
}

static double
time_wikisort(int elts, SORT_TYPE *array)
{
	SORT_TYPE *testarray = malloc(sizeof(SORT_TYPE) * elts);
	memcpy(testarray, array, elts * sizeof(SORT_TYPE));
	clock_t sum = 0;
	for (int i = 0; i < Trials; i++) {
		clock_t begin = clock();
		WikiSort(testarray, (size_t)elts, sizeof(SORT_TYPE), SORT_CMP);
		clock_t end = clock();
		sum += (end - begin);
	}
	clock_t average = sum / (clock_t)Trials;
	free(testarray);
	return (double)average / CLOCKS_PER_SEC;
}

static void
time_bench(int elts, SORT_TYPE *array)
{
	double mtime = time_mergesort(elts, array);
	double wtime = time_wikisort(elts, array);
	printf("%-12d%-16f%8f\n", elts, mtime, wtime);
}

static void
rand_bench(int elts)
{
	SORT_TYPE *array = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		array[i] = rand();
	}
	time_bench(elts, array);
	free(array);
}

static void
sort_bench(int elts)
{
	SORT_TYPE *array = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		array[i] = i;
	}
	time_bench(elts, array);
	free(array);
}

static void
partial_bench(int elts)
{
	SORT_TYPE *array = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		if (i <= elts / 2)
			array[i] = i;
		else
			array[i] = rand();
	}
	time_bench(elts, array);
	free(array);
}

static void
reverse_bench(int elts)
{
	SORT_TYPE *array = malloc(sizeof(SORT_TYPE) * elts);
	for (int i = 0; i < elts; i++) {
		array[i] = elts - i;
	}
	time_bench(elts, array);
	free(array);
}

static void
run_bench(enum test t)
{
	printf("|----------------------------------------------------|\n");
	switch (t) {
		case RAND:
			printf("Benchmarks for a randomly generated array\n");
			break;
		case SORT:
			printf("Benchmarks for a sorted array\n");
			break;
		case PART:
			printf("Benchmarks for a partially sorted array\n");
			break;
		case REV:
			printf("Benchmarks for a reversed array\n");
			break;
	}
	printf("%-12s%-16s%8s\n", "Size", "Original", "Wikisort");
	Trials = TRIALS;
	int base = pow(2, BASE);
	int max = pow(2, MAX);
	for (int elts = base; elts < max; elts *= 2) {
		switch (t) {
			case RAND:
				rand_bench(elts);
				break;
			case SORT:
				sort_bench(elts);
				break;
			case PART:
				partial_bench(elts);
				break;
			case REV:
				reverse_bench(elts);
				break;
		}
		if (Trials > MIN_TRIALS)
			Trials--;
	}
}
/*
 * Tests the running time in seconds of WikiSort as compared to stdlib's
 * mergesort implementation
 */
int
main()
{
	printf("Benchmarking... (Time is in seconds)\n");
	srand(time(NULL));  // randomize seed
	for (int test = 0; test < TESTS; test++) {
		run_bench(test);
	}
	printf("|----------------------------------------------------|\n");
}
