/***********************************************************
 WikiSort (public domain license)
 https://github.com/BonzaiThePenguin/WikiSort

 to run:
 clang -o WikiSort.x WikiSort.c -O3
 (or replace 'clang' with 'gcc')
 ./WikiSort.x
***********************************************************/

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "WikiHelpers.h"

/* bottom-up merge sort combined with an in-place merge algorithm for O(1) memory use */
int WikiSort(void *arr, size_t nmemb, size_t size, Comparison compare) {
	char *array = (char *)arr;
/* use a small cache to speed up some of the operations */
#if DYNAMIC_CACHE
	size_t cache_size;
	char *cache = NULL;
#else
/* since the cache size is fixed, it's still O(1) memory! */
/* just keep in mind that making it too small ruins the point (nothing will fit into it), */
/* and making it too large also ruins the point (so much for "low memory"!) */
/* removing the cache entirely still gives 70% of the performance of a standard merge */
#define CACHE_SIZE 512
	const size_t cache_size = CACHE_SIZE;
	char *cache = (char *)malloc(CACHE_SIZE * size);
#endif

	WikiIterator iterator;

	/* if the array is of size 0, 1, 2, or 3, just sort them like so: */
	if (nmemb < 4) {
		if (nmemb == 3) {
			/* hard-coded insertion sort */
			if (COMPARE(Index_addr(array, 1), Index_addr(array, 0)))
				Swap(Index_addr(array, 0), Index_addr(array, 1), size);
			if (COMPARE(Index_addr(array, 2), Index_addr(array, 1))) {
				Swap(Index_addr(array, 1), Index_addr(array, 2), size);
				if (COMPARE(Index_addr(array, 1), Index_addr(array, 0)))
					Swap(Index_addr(array, 0), Index_addr(array, 1), size);
			}
		} else if (nmemb == 2) {
			/* swap the items if they're out of order */
			if (COMPARE(Index_addr(array, 1), Index_addr(array, 0)))
				Swap(Index_addr(array, 0), Index_addr(array, 1), size);
		}

		return 0;
	}

	/* sort groups of 4-8 items at a time using an unstable sorting network, */
	/* but keep track of the original item orders to force it to be stable */
	/* http://pages.ripco.net/~jgamble/nw.html */
	iterator = WikiIterator_new(nmemb, 4);
	WikiIterator_begin(&iterator);
	while (!WikiIterator_finished(&iterator)) {
		uint8_t order[] = {0, 1, 2, 3, 4, 5, 6, 7};
		Range range = WikiIterator_nextRange(&iterator);

#define SWAP(x, y)                                                                                                                                                                                          \
	if (COMPARE(Index_addr(array, range.start + y), Index_addr(array, range.start + x)) || (order[x] > order[y] && !COMPARE(Index_addr(array, range.start + x), Index_addr(array, range.start + y)))) { \
		Swap(Index_addr(array, range.start + x), Index_addr(array, range.start + y), size);                                                                                                               \
		Swap(&(order[x]), &(order[y]), sizeof(uint8_t));                                                                                                                                                                   \
	}

		if (Range_length(range) == 8) {
			SWAP(0, 1);
			SWAP(2, 3);
			SWAP(4, 5);
			SWAP(6, 7);
			SWAP(0, 2);
			SWAP(1, 3);
			SWAP(4, 6);
			SWAP(5, 7);
			SWAP(1, 2);
			SWAP(5, 6);
			SWAP(0, 4);
			SWAP(3, 7);
			SWAP(1, 5);
			SWAP(2, 6);
			SWAP(1, 4);
			SWAP(3, 6);
			SWAP(2, 4);
			SWAP(3, 5);
			SWAP(3, 4);

		} else if (Range_length(range) == 7) {
			SWAP(1, 2);
			SWAP(3, 4);
			SWAP(5, 6);
			SWAP(0, 2);
			SWAP(3, 5);
			SWAP(4, 6);
			SWAP(0, 1);
			SWAP(4, 5);
			SWAP(2, 6);
			SWAP(0, 4);
			SWAP(1, 5);
			SWAP(0, 3);
			SWAP(2, 5);
			SWAP(1, 3);
			SWAP(2, 4);
			SWAP(2, 3);

		} else if (Range_length(range) == 6) {
			SWAP(1, 2);
			SWAP(4, 5);
			SWAP(0, 2);
			SWAP(3, 5);
			SWAP(0, 1);
			SWAP(3, 4);
			SWAP(2, 5);
			SWAP(0, 3);
			SWAP(1, 4);
			SWAP(2, 4);
			SWAP(1, 3);
			SWAP(2, 3);

		} else if (Range_length(range) == 5) {
			SWAP(0, 1);
			SWAP(3, 4);
			SWAP(2, 4);
			SWAP(2, 3);
			SWAP(1, 4);
			SWAP(0, 3);
			SWAP(0, 2);
			SWAP(1, 3);
			SWAP(1, 2);

		} else if (Range_length(range) == 4) {
			SWAP(0, 1);
			SWAP(2, 3);
			SWAP(0, 2);
			SWAP(1, 3);
			SWAP(1, 2);
		}
	}
	if (nmemb < 8)
		return 0;

#if DYNAMIC_CACHE
	/* good choices for the cache size are: */
	/* (size + 1)/2 – turns into a full-speed standard merge sort since everything fits into the cache */
	cache_size = (nmemb + 1) / 2;
	cache = (char *)malloc(cache_size * size);

	if (!cache) {
		/* sqrt((size + 1)/2) + 1 – this will be the size of the A blocks at the largest level of merges, */
		/* so a buffer of this size would allow it to skip using internal or in-place merges for anything */
		cache_size = sqrt(cache_size) + 1;
		cache = (char *)malloc(cache_size * size);

		if (!cache) {
			/* 512 – chosen from careful testing as a good balance between fixed-size memory use and run time */
			if (cache_size > 512) {
				cache_size = 512;
				cache = (char *)malloc(cache_size * size);
			}

			/* 0 – if the system simply cannot allocate any extra memory whatsoever, no memory works just fine */
			if (!cache)
				cache_size = 0;
		}
	}
#endif

	/* then merge sort the higher levels, which can be 8-15, 16-31, 32-63, 64-127, etc. */
	while (true) {
		/* if every A and B block will fit into the cache, use a special branch specifically for merging with the cache */
		/* (we use < rather than <= since the block size might be one more than iterator.length()) */
		if (WikiIterator_length(&iterator) < cache_size) {
			/* if four subarrays fit into the cache, it's faster to merge both pairs of subarrays into the cache, */
			/* then merge the two merged subarrays from the cache back into the original array */
			if ((WikiIterator_length(&iterator) + 1) * 4 <= cache_size && WikiIterator_length(&iterator) * 4 <= nmemb) {
				WikiIterator_begin(&iterator);
				while (!WikiIterator_finished(&iterator)) {
					/* merge A1 and B1 into the cache */
					Range A1, B1, A2, B2, A3, B3;
					A1 = WikiIterator_nextRange(&iterator);
					B1 = WikiIterator_nextRange(&iterator);
					A2 = WikiIterator_nextRange(&iterator);
					B2 = WikiIterator_nextRange(&iterator);

					if (COMPARE(Index_addr(array, B1.end - 1), Index_addr(array, A1.start))) {
						/* the two ranges are in reverse order, so copy them in reverse order into the cache */
						memcpy(Index_addr(cache, Range_length(B1)), Index_addr(array, A1.start), Range_length(A1) * size);
						memcpy(Index_addr(cache, 0), Index_addr(array, B1.start), Range_length(B1) * size);
					} else if (COMPARE(Index_addr(array, B1.start), Index_addr(array, A1.end - 1))) {
						/* these two ranges weren't already in order, so merge them into the cache */
						MergeInto(array, A1, B1, size, compare, Index_addr(cache, 0));
					} else {
						/* if A1, B1, A2, and B2 are all in order, skip doing anything else */
						if (!COMPARE(Index_addr(array, B2.start), Index_addr(array, A2.end - 1)) && !COMPARE(Index_addr(array, A2.start), Index_addr(array, B1.end - 1)))
							continue;

						/* copy A1 and B1 into the cache in the same order */
						memcpy(Index_addr(cache, 0), Index_addr(array, A1.start), Range_length(A1) * size);
						memcpy(Index_addr(cache, Range_length(A1)), Index_addr(array, B1.start), Range_length(B1) * size);
					}
					A1 = Range_new(A1.start, B1.end);

					/* merge A2 and B2 into the cache */
					if (COMPARE(Index_addr(array, B2.end - 1), Index_addr(array, A2.start))) {
						/* the two ranges are in reverse order, so copy them in reverse order into the cache */
						memcpy(Index_addr(cache, Range_length(A1) + Range_length(B2)), Index_addr(array, A2.start), Range_length(A2) * size);
						memcpy(Index_addr(cache, Range_length(A1)), Index_addr(array, B2.start), Range_length(B2) * size);
					} else if (COMPARE(Index_addr(array, B2.start), Index_addr(array, A2.end - 1))) {
						/* these two ranges weren't already in order, so merge them into the cache */
						MergeInto(array, A2, B2, size, compare, Index_addr(cache, Range_length(A1)));
					} else {
						/* copy A2 and B2 into the cache in the same order */
						memcpy(Index_addr(cache, Range_length(A1)), Index_addr(array, A2.start), Range_length(A2) * size);
						memcpy(Index_addr(cache, Range_length(A1) + Range_length(A2)), Index_addr(array, B2.start), Range_length(B2) * size);
					}
					A2 = Range_new(A2.start, B2.end);

					/* merge A1 and A2 from the cache into the array */
					A3 = Range_new(0, Range_length(A1));
					B3 = Range_new(Range_length(A1), Range_length(A1) + Range_length(A2));

					if (COMPARE(Index_addr(cache, B3.end - 1), Index_addr(cache, A3.start))) {
						/* the two ranges are in reverse order, so copy them in reverse order into the array */
						memcpy(Index_addr(array, A1.start + Range_length(A2)), Index_addr(cache, A3.start), Range_length(A3) * size);
						memcpy(Index_addr(array, A1.start), Index_addr(cache, B3.start), Range_length(B3) * size);
					} else if (COMPARE(Index_addr(cache, B3.start), Index_addr(cache, A3.end - 1))) {
						/* these two ranges weren't already in order, so merge them back into the array */
						MergeInto(cache, A3, B3, size, compare, Index_addr(array, A1.start));
					} else {
						/* copy A3 and B3 into the array in the same order */
						memcpy(Index_addr(array, A1.start), Index_addr(cache, A3.start), Range_length(A3) * size);
						memcpy(Index_addr(array, A1.start + Range_length(A1)), Index_addr(cache, B3.start), Range_length(B3) * size);
					}
				}

				/* we merged two levels at the same time, so we're done with this level already */
				/* (iterator.nextLevel() is called again at the bottom of this outer merge loop) */
				WikiIterator_nextLevel(&iterator);

			} else {
				WikiIterator_begin(&iterator);
				while (!WikiIterator_finished(&iterator)) {
					Range A = WikiIterator_nextRange(&iterator);
					Range B = WikiIterator_nextRange(&iterator);

					if (COMPARE(Index_addr(array, B.end - 1), Index_addr(array, A.start))) {
						/* the two ranges are in reverse order, so a simple rotation should fix it */
						Rotate(array, Range_length(A), Range_new(A.start, B.end), cache, cache_size, size);
					} else if (COMPARE(Index_addr(array, B.start), Index_addr(array, A.end - 1))) {
						/* these two ranges weren't already in order, so we'll need to merge them! */
						memcpy(Index_addr(cache, 0), Index_addr(array, A.start), Range_length(A) * size);
						MergeExternal(array, A, B, size, compare, cache);
					}
				}
			}
		} else {
			/* this is where the in-place merge logic starts!
			 1. pull out two internal buffers each containing √A unique values
				1a. adjust block_size and buffer_size if we couldn't find enough unique values
			 2. loop over the A and B subarrays within this level of the merge sort
			 3. break A and B into blocks of size 'block_size'
			 4. "tag" each of the A blocks with values from the first internal buffer
			 5. roll the A blocks through the B blocks and drop/rotate them where they belong
			 6. merge each A block with any B values that follow, using the cache or the second internal buffer
			 7. sort the second internal buffer if it exists
			 8. redistribute the two internal buffers back into the array */

			size_t block_size = sqrt(WikiIterator_length(&iterator));
			size_t buffer_size = WikiIterator_length(&iterator) / block_size + 1;

			/* as an optimization, we really only need to pull out the internal buffers once for each level of merges */
			/* after that we can reuse the same buffers over and over, then redistribute it when we're finished with this level */
			Range buffer1, buffer2, A, B;
			bool find_separately;
			size_t index, last, count, find, start, pull_index = 0;
			struct {
				size_t from, to, count;
				Range range;
			} pull[2];
			pull[0].from = pull[0].to = pull[0].count = 0;
			pull[0].range = Range_new(0, 0);
			pull[1].from = pull[1].to = pull[1].count = 0;
			pull[1].range = Range_new(0, 0);

			buffer1 = Range_new(0, 0);
			buffer2 = Range_new(0, 0);

			/* find two internal buffers of size 'buffer_size' each */
			find = buffer_size + buffer_size;
			find_separately = false;

			if (block_size <= cache_size) {
				/* if every A block fits into the cache then we won't need the second internal buffer, */
				/* so we really only need to find 'buffer_size' unique values */
				find = buffer_size;
			} else if (find > WikiIterator_length(&iterator)) {
				/* we can't fit both buffers into the same A or B subarray, so find two buffers separately */
				find = buffer_size;
				find_separately = true;
			}

			/* we need to find either a single contiguous space containing 2√A unique values (which will be split up into two buffers of size √A each), */
			/* or we need to find one buffer of < 2√A unique values, and a second buffer of √A unique values, */
			/* OR if we couldn't find that many unique values, we need the largest possible buffer we can get */

			/* in the case where it couldn't find a single buffer of at least √A unique values, */
			/* all of the Merge steps must be replaced by a different merge algorithm (MergeInPlace) */
			WikiIterator_begin(&iterator);
			while (!WikiIterator_finished(&iterator)) {
				A = WikiIterator_nextRange(&iterator);
				B = WikiIterator_nextRange(&iterator);

/* just store information about where the values will be pulled from and to, */
/* as well as how many values there are, to create the two internal buffers */
#define PULL(_to)                                           \
	pull[pull_index].range = Range_new(A.start, B.end); \
	pull[pull_index].count = count;                     \
	pull[pull_index].from = index;                      \
	pull[pull_index].to = _to

				/* check A for the number of unique values we need to fill an internal buffer */
				/* these values will be pulled out to the start of A */
				for (last = A.start, count = 1; count < find; last = index, count++) {
					index = FindLastForward(array, Index_addr(array, last), Range_new(last + 1, A.end), size, compare, find - count);
					if (index == A.end)
						break;
				}
				index = last;

				if (count >= buffer_size) {
					/* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
					PULL(A.start);
					pull_index = 1;

					if (count == buffer_size + buffer_size) {
						/* we were able to find a single contiguous section containing 2√A unique values, */
						/* so this section can be used to contain both of the internal buffers we'll need */
						buffer1 = Range_new(A.start, A.start + buffer_size);
						buffer2 = Range_new(A.start + buffer_size, A.start + count);
						break;
					} else if (find == buffer_size + buffer_size) {
						/* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
						/* so we still need to find a second separate buffer of at least √A unique values */
						buffer1 = Range_new(A.start, A.start + count);
						find = buffer_size;
					} else if (block_size <= cache_size) {
						/* we found the first and only internal buffer that we need, so we're done! */
						buffer1 = Range_new(A.start, A.start + count);
						break;
					} else if (find_separately) {
						/* found one buffer, but now find the other one */
						buffer1 = Range_new(A.start, A.start + count);
						find_separately = false;
					} else {
						/* we found a second buffer in an 'A' subarray containing √A unique values, so we're done! */
						buffer2 = Range_new(A.start, A.start + count);
						break;
					}
				} else if (pull_index == 0 && count > Range_length(buffer1)) {
					/* keep track of the largest buffer we were able to find */
					buffer1 = Range_new(A.start, A.start + count);
					PULL(A.start);
				}

				/* check B for the number of unique values we need to fill an internal buffer */
				/* these values will be pulled out to the end of B */
				for (last = B.end - 1, count = 1; count < find; last = index - 1, count++) {
					index = FindFirstBackward(array, Index_addr(array, last), Range_new(B.start, last), size, compare, find - count);
					if (index == B.start)
						break;
				}
				index = last;

				if (count >= buffer_size) {
					/* keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer */
					PULL(B.end);
					pull_index = 1;

					if (count == buffer_size + buffer_size) {
						/* we were able to find a single contiguous section containing 2√A unique values, */
						/* so this section can be used to contain both of the internal buffers we'll need */
						buffer1 = Range_new(B.end - count, B.end - buffer_size);
						buffer2 = Range_new(B.end - buffer_size, B.end);
						break;
					} else if (find == buffer_size + buffer_size) {
						/* we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values, */
						/* so we still need to find a second separate buffer of at least √A unique values */
						buffer1 = Range_new(B.end - count, B.end);
						find = buffer_size;
					} else if (block_size <= cache_size) {
						/* we found the first and only internal buffer that we need, so we're done! */
						buffer1 = Range_new(B.end - count, B.end);
						break;
					} else if (find_separately) {
						/* found one buffer, but now find the other one */
						buffer1 = Range_new(B.end - count, B.end);
						find_separately = false;
					} else {
						/* buffer2 will be pulled out from a 'B' subarray, so if the first buffer was pulled out from the corresponding 'A' subarray, */
						/* we need to adjust the end point for that A subarray so it knows to stop redistributing its values before reaching buffer2 */
						if (pull[0].range.start == A.start)
							pull[0].range.end -= pull[1].count;

						/* we found a second buffer in an 'B' subarray containing √A unique values, so we're done! */
						buffer2 = Range_new(B.end - count, B.end);
						break;
					}
				} else if (pull_index == 0 && count > Range_length(buffer1)) {
					/* keep track of the largest buffer we were able to find */
					buffer1 = Range_new(B.end - count, B.end);
					PULL(B.end);
				}
			}

			/* pull out the two ranges so we can use them as internal buffers */
			for (pull_index = 0; pull_index < 2; pull_index++) {
				Range range;
				size_t length = pull[pull_index].count;

				if (pull[pull_index].to < pull[pull_index].from) {
					/* we're pulling the values out to the left, which means the start of an A subarray */
					index = pull[pull_index].from;
					for (count = 1; count < length; count++) {
						index = FindFirstBackward(array, Index_addr(array, index - 1), Range_new(pull[pull_index].to, pull[pull_index].from - (count - 1)), size, compare, length - count);
						range = Range_new(index + 1, pull[pull_index].from + 1);
						Rotate(array, Range_length(range) - count, range, cache, cache_size, size);
						pull[pull_index].from = index + count;
					}
				} else if (pull[pull_index].to > pull[pull_index].from) {
					/* we're pulling values out to the right, which means the end of a B subarray */
					index = pull[pull_index].from + 1;
					for (count = 1; count < length; count++) {
						index = FindLastForward(array, Index_addr(array, index), Range_new(index, pull[pull_index].to), size, compare, length - count);
						range = Range_new(pull[pull_index].from, index - 1);
						Rotate(array, count, range, cache, cache_size, size);
						pull[pull_index].from = index - 1 - count;
					}
				}
			}

			/* adjust block_size and buffer_size based on the values we were able to pull out */
			buffer_size = Range_length(buffer1);
			block_size = WikiIterator_length(&iterator) / buffer_size + 1;

			/* the first buffer NEEDS to be large enough to tag each of the evenly sized A blocks, */
			/* so this was originally here to test the math for adjusting block_size above */
			/* assert((WikiIterator_length(&iterator) + 1)/block_size <= buffer_size); */

			/* now that the two internal buffers have been created, it's time to merge each A+B combination at this level of the merge sort! */
			WikiIterator_begin(&iterator);
			while (!WikiIterator_finished(&iterator)) {
				A = WikiIterator_nextRange(&iterator);
				B = WikiIterator_nextRange(&iterator);

				/* remove any parts of A or B that are being used by the internal buffers */
				start = A.start;
				if (start == pull[0].range.start) {
					if (pull[0].from > pull[0].to) {
						A.start += pull[0].count;

						/* if the internal buffer takes up the entire A or B subarray, then there's nothing to merge */
						/* this only happens for very small subarrays, like √4 = 2, 2 * (2 internal buffers) = 4, */
						/* which also only happens when cache_size is small or 0 since it'd otherwise use MergeExternal */
						if (Range_length(A) == 0)
							continue;
					} else if (pull[0].from < pull[0].to) {
						B.end -= pull[0].count;
						if (Range_length(B) == 0)
							continue;
					}
				}
				if (start == pull[1].range.start) {
					if (pull[1].from > pull[1].to) {
						A.start += pull[1].count;
						if (Range_length(A) == 0)
							continue;
					} else if (pull[1].from < pull[1].to) {
						B.end -= pull[1].count;
						if (Range_length(B) == 0)
							continue;
					}
				}

				if (COMPARE(Index_addr(array, B.end - 1), Index_addr(array, A.start))) {
					/* the two ranges are in reverse order, so a simple rotation should fix it */
					Rotate(array, Range_length(A), Range_new(A.start, B.end), cache, cache_size, size);
				} else if (COMPARE(Index_addr(array, A.end), Index_addr(array, A.end - 1))) {
					/* these two ranges weren't already in order, so we'll need to merge them! */
					Range blockA, firstA, lastA, lastB, blockB;
					size_t indexA, findA;

					/* break the remainder of A into blocks. firstA is the uneven-sized first A block */
					blockA = Range_new(A.start, A.end);
					firstA = Range_new(A.start, A.start + Range_length(blockA) % block_size);

					/* swap the first value of each A block with the value in buffer1 */
					for (indexA = buffer1.start, index = firstA.end; index < blockA.end; indexA++, index += block_size) Swap(Index_addr(array, indexA), Index_addr(array, index), size);

					/* start rolling the A blocks through the B blocks! */
					/* whenever we leave an A block behind, we'll need to merge the previous A block with any B blocks that follow it, so track that information as well */
					lastA = firstA;
					lastB = Range_new(0, 0);
					blockB = Range_new(B.start, B.start + Min(block_size, Range_length(B)));
					blockA.start += Range_length(firstA);
					indexA = buffer1.start;

					/* if the first unevenly sized A block fits into the cache, copy it there for when we go to Merge it */
					/* otherwise, if the second buffer is available, block swap the contents into that */
					if (Range_length(lastA) <= cache_size)
						memcpy(Index_addr(cache, 0), Index_addr(array, lastA.start), Range_length(lastA) * size);
					else if (Range_length(buffer2) > 0)
						BlockSwap(array, lastA.start, buffer2.start, Range_length(lastA), size);

					if (Range_length(blockA) > 0) {
						while (true) {
							/* if there's a previous B block and the first value of the minimum A block is <= the last value of the previous B block, */
							/* then drop that minimum A block behind. or if there are no B blocks left then keep dropping the remaining A blocks. */
							if ((Range_length(lastB) > 0 && !COMPARE(Index_addr(array, lastB.end - 1), Index_addr(array, indexA))) || Range_length(blockB) == 0) {
								/* figure out where to split the previous B block, and rotate it at the split */
								size_t B_split = BinaryFirst(array, Index_addr(array, indexA), lastB, size, compare);
								size_t B_remaining = lastB.end - B_split;

								/* swap the minimum A block to the beginning of the rolling A blocks */
								size_t minA = blockA.start;
								for (findA = minA + block_size; findA < blockA.end; findA += block_size)
									if (COMPARE(Index_addr(array, findA), Index_addr(array, minA)))
										minA = findA;
								BlockSwap(array, blockA.start, minA, block_size, size);

								/* swap the first item of the previous A block back with its original value, which is stored in buffer1 */
								Swap(Index_addr(array, blockA.start), Index_addr(array, indexA), size);
								indexA++;

								/*
								 locally merge the previous A block with the B values that follow it
								 if lastA fits into the external cache we'll use that (with MergeExternal),
								 or if the second internal buffer exists we'll use that (with MergeInternal),
								 or failing that we'll use a strictly in-place merge algorithm (MergeInPlace)
								 */
								if (Range_length(lastA) <= cache_size)
									MergeExternal(array, lastA, Range_new(lastA.end, B_split), size, compare, cache);
								else if (Range_length(buffer2) > 0)
									MergeInternal(array, lastA, Range_new(lastA.end, B_split), size, compare, buffer2);
								else
									MergeInPlace(array, lastA, Range_new(lastA.end, B_split), size, compare, cache, cache_size);

								if (Range_length(buffer2) > 0 || block_size <= cache_size) {
									/* copy the previous A block into the cache or buffer2, since that's where we need it to be when we go to merge it anyway */
									if (block_size <= cache_size)
										memcpy(Index_addr(cache, 0), Index_addr(array, blockA.start), block_size * size);
									else
										BlockSwap(array, blockA.start, buffer2.start, block_size, size);

									/* this is equivalent to rotating, but faster */
									/* the area normally taken up by the A block is either the contents of buffer2, or data we don't need anymore since we memcopied it */
									/* either way, we don't need to retain the order of those items, so instead of rotating we can just block swap B to where it belongs */
									BlockSwap(array, B_split, blockA.start + block_size - B_remaining, B_remaining, size);
								} else {
									/* we are unable to use the 'buffer2' trick to speed up the rotation operation since buffer2 doesn't exist, so perform a normal rotation */
									Rotate(array, blockA.start - B_split, Range_new(B_split, blockA.start + block_size), cache, cache_size, size);
								}

								/* update the range for the remaining A blocks, and the range remaining from the B block after it was split */
								lastA = Range_new(blockA.start - B_remaining, blockA.start - B_remaining + block_size);
								lastB = Range_new(lastA.end, lastA.end + B_remaining);

								/* if there are no more A blocks remaining, this step is finished! */
								blockA.start += block_size;
								if (Range_length(blockA) == 0)
									break;

							} else if (Range_length(blockB) < block_size) {
								/* move the last B block, which is unevenly sized, to before the remaining A blocks, by using a rotation */
								/* the cache is disabled here since it might contain the contents of the previous A block */
								Rotate(array, blockB.start - blockA.start, Range_new(blockA.start, blockB.end), cache, 0, size);

								lastB = Range_new(blockA.start, blockA.start + Range_length(blockB));
								blockA.start += Range_length(blockB);
								blockA.end += Range_length(blockB);
								blockB.end = blockB.start;
							} else {
								/* roll the leftmost A block to the end by swapping it with the next B block */
								BlockSwap(array, blockA.start, blockB.start, block_size, size);
								lastB = Range_new(blockA.start, blockA.start + block_size);

								blockA.start += block_size;
								blockA.end += block_size;
								blockB.start += block_size;

								if (blockB.end > B.end - block_size)
									blockB.end = B.end;
								else
									blockB.end += block_size;
							}
						}
					}

					/* merge the last A block with the remaining B values */
					if (Range_length(lastA) <= cache_size)
						MergeExternal(array, lastA, Range_new(lastA.end, B.end), size, compare, cache);
					else if (Range_length(buffer2) > 0)
						MergeInternal(array, lastA, Range_new(lastA.end, B.end), size, compare, buffer2);
					else
						MergeInPlace(array, lastA, Range_new(lastA.end, B.end), size, compare, cache, cache_size);
				}
			}

			/* when we're finished with this merge step we should have the one or two internal buffers left over, where the second buffer is all jumbled up */
			/* insertion sort the second buffer, then redistribute the buffers back into the array using the opposite process used for creating the buffer */

			/* while an unstable sort like quicksort could be applied here, in benchmarks it was consistently slightly slower than a simple insertion sort, */
			/* even for tens of millions of items. this may be because insertion sort is quite fast when the data is already somewhat sorted, like it is here */
			InsertionSort(array, buffer2, size, compare);

			for (pull_index = 0; pull_index < 2; pull_index++) {
				size_t amount, unique = pull[pull_index].count * 2;
				if (pull[pull_index].from > pull[pull_index].to) {
					/* the values were pulled out to the left, so redistribute them back to the right */
					Range buffer = Range_new(pull[pull_index].range.start, pull[pull_index].range.start + pull[pull_index].count);
					while (Range_length(buffer) > 0) {
						index = FindFirstForward(array, Index_addr(array, buffer.start), Range_new(buffer.end, pull[pull_index].range.end), size, compare, unique);
						amount = index - buffer.end;
						Rotate(array, Range_length(buffer), Range_new(buffer.start, index), cache, cache_size, size);
						buffer.start += (amount + 1);
						buffer.end += amount;
						unique -= 2;
					}
				} else if (pull[pull_index].from < pull[pull_index].to) {
					/* the values were pulled out to the right, so redistribute them back to the left */
					Range buffer = Range_new(pull[pull_index].range.end - pull[pull_index].count, pull[pull_index].range.end);
					while (Range_length(buffer) > 0) {
						index = FindLastBackward(array, Index_addr(array, buffer.end - 1), Range_new(pull[pull_index].range.start, buffer.start), size, compare, unique);
						amount = buffer.start - index;
						Rotate(array, amount, Range_new(index, buffer.end), cache, cache_size, size);
						buffer.start -= amount;
						buffer.end -= (amount + 1);
						unique -= 2;
					}
				}
			}
		}

		/* double the size of each A and B subarray that will be merged in the next level */
		if (!WikiIterator_nextLevel(&iterator))
			break;
	}

#if DYNAMIC_CACHE
	if (cache)
		free(cache);
#endif
	return 0;
#undef CACHE_SIZE
}
