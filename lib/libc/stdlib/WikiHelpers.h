/* whether to give WikiSort a full-size cache, to see how it performs when given more memory */
#define DYNAMIC_CACHE false

#define Var(name, value)				__typeof__(value) name = value
#define Allocate(type, count)				(type *)malloc((count) * sizeof(type))
// Gets the address of a given index in an array
#define Index_addr(base, ind) ((base) + ((size) * (ind)))
#define COMPARE(arg1, arg2) ((compare(arg1, arg2) == 1))

size_t Min(const size_t a, const size_t b) {
	if (a < b) return a;
	return b;
}

size_t Max(const size_t a, const size_t b) {
	if (a > b) return a;
	return b;
}

typedef int (*Comparison)(const void *, const void *);

/* structure to represent ranges within the array */
typedef struct {
	size_t start;
	size_t end;
} Range;

size_t Range_length(Range range) { return range.end - range.start; }

Range Range_new(const size_t start, const size_t end) {
	Range range;
	range.start = start;
	range.end = end;
	return range;
}

/* toolbox functions used by the sorter */

void Swap(void *va, void *vb, size_t s)
{
  uint8_t t, *a = (uint8_t*)va, *b = (uint8_t*)vb;
  while(s--)
    t = a[s], a[s] = b[s], b[s] = t;
}

/* 63 -> 32, 64 -> 64, etc. */
/* this comes from Hacker's Delight */
/* TODO Generalize this */
size_t FloorPowerOfTwo (const size_t value) {
	size_t x = value;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
#if __LP64__
	x = x | (x >> 32);
#endif
	return x - (x >> 1);
}

/* find the index of the first value within the range that is equal to array[index] */
size_t BinaryFirst(const void *arr, const void *value, const Range range, size_t size, const Comparison compare) {
	const char *array = (const char *)arr;
	size_t start = range.start, end = range.end - 1;
	if (range.start >= range.end) return range.start;
	while (start < end) {
		size_t mid = start + (end - start)/2;
		if (COMPARE(Index_addr(array, mid), value))
			start = mid + 1;
		else
			end = mid;
	}
	if (start == range.end - 1 && COMPARE(Index_addr(array, start), value)) start++;
	return start;
}

/* find the index of the last value within the range that is equal to array[index], plus 1 */
size_t BinaryLast(const void *arr, const void *value, const Range range, size_t size, const Comparison compare) {
	const char *array = (const char *)arr;
	size_t start = range.start, end = range.end - 1;
	if (range.start >= range.end) return range.end;
	while (start < end) {
		size_t mid = start + (end - start)/2;
		if (!COMPARE(value, Index_addr(array, mid)))
			start = mid + 1;
		else
			end = mid;
	}
	if (start == range.end - 1 && !COMPARE(value, Index_addr(array, start))) start++;
	return start;
}

/* combine a linear search with a binary search to reduce the number of comparisons in situations */
/* where have some idea as to how many unique values there are and where the next value might be */
size_t FindFirstForward(const void *arr, const void *value, const Range range, size_t size, const Comparison compare, const size_t unique) {
	const char *array = (const char *)arr;
	size_t skip, index;
	if (Range_length(range) == 0) return range.start;
	skip = Max(Range_length(range)/unique, 1);

	for (index = range.start + skip; COMPARE(Index_addr(array, index - 1), value); index += skip)
		if (index >= range.end - skip)
			return BinaryFirst(array, value, Range_new(index, range.end), size, compare);

	return BinaryFirst(array, value, Range_new(index - skip, index), size, compare);
}

size_t FindLastForward(const void *arr, const void *value, const Range range, size_t size, const Comparison compare, const size_t unique) {
	const char *array = (const char *)arr;
	size_t skip, index;
	if (Range_length(range) == 0) return range.start;
	skip = Max(Range_length(range)/unique, 1);

	for (index = range.start + skip; !COMPARE(value, Index_addr(array, index - 1)); index += skip)
		if (index >= range.end - skip)
			return BinaryLast(array, value, Range_new(index, range.end), size, compare);

	return BinaryLast(array, value, Range_new(index - skip, index), size, compare);
}

size_t FindFirstBackward(const void *arr, const void *value, const Range range, size_t size, const Comparison compare, const size_t unique) {
	const char *array = (const char *)arr;
	size_t skip, index;
	if (Range_length(range) == 0) return range.start;
	skip = Max(Range_length(range)/unique, 1);

	for (index = range.end - skip; index > range.start && !COMPARE(Index_addr(array, index - 1), value); index -= skip)
		if (index < range.start + skip)
			return BinaryFirst(array, value, Range_new(range.start, index), size, compare);

	return BinaryFirst(array, value, Range_new(index, index + skip), size, compare);
}

size_t FindLastBackward(const void *arr, const void *value, const Range range, size_t size, const Comparison compare, const size_t unique) {
	const char *array = (const char *)arr;
	size_t skip, index;
	if (Range_length(range) == 0) return range.start;
	skip = Max(Range_length(range)/unique, 1);

	for (index = range.end - skip; index > range.start && COMPARE(value, Index_addr(array, index - 1)); index -= skip)
		if (index < range.start + skip)
			return BinaryLast(array, value, Range_new(range.start, index), size, compare);

	return BinaryLast(array, value, Range_new(index, index + skip), size, compare);
}

/*
 * Generic insertionsort borrowed from merge.c
 */
void InsertionSort(void *arr, const Range range, size_t size, const Comparison compare) {
	char *a = (char *)arr;
	char *ai, *s, *t, *u, tmp;
	int i;

	a = Index_addr(a, range.start);
	size_t n = range.end - range.start;
	for (ai = a + size; --n >= 1; ai += size)
		for (t = ai; t > a; t -= size) {
			u = t - size;
			if (compare(t, u) != 1) break;
			Swap(u, t, size);
		}
}

/* reverse a range of values within the array */
void Reverse(void *arr, const Range range, size_t size) {
	char *array = (char *)arr;
	size_t index;
	for (index = Range_length(range)/2; index > 0; index--)
		Swap(Index_addr(array, range.start + index - 1), Index_addr(array, range.end - index), size);
}

/* swap a series of values in the array */
void BlockSwap(void *arr, const size_t start1, const size_t start2, const size_t block_size, size_t size) {
	char *array = (char *)arr;
	size_t index;
	for (index = 0; index < block_size; index++)
		Swap(Index_addr(array, start1 + index), Index_addr(array, start2 + index), size);
}

/* rotate the values in an array ([0 1 2 3] becomes [1 2 3 0] if we rotate by 1) */
/* this assumes that 0 <= amount <= range.length() */
void Rotate(void *arr, const size_t amount, const Range range, void *cache_base, const size_t cache_size, size_t size) {
	char *array = (char *)arr;
	char *cache = (char *)cache_base;
	size_t split; Range range1, range2;
	if (Range_length(range) == 0) return;

	split = range.start + amount;
	range1 = Range_new(range.start, split);
	range2 = Range_new(split, range.end);

	/* if the smaller of the two ranges fits into the cache, it's *slightly* faster copying it there and shifting the elements over */
	if (Range_length(range1) <= Range_length(range2)) {
		if (Range_length(range1) <= cache_size) {
			memcpy(Index_addr(cache, 0), Index_addr(array, range1.start), Range_length(range1) * size);
			memmove(Index_addr(array, range1.start), Index_addr(array, range2.start), Range_length(range2) * size);
			memcpy(Index_addr(array, range1.start + Range_length(range2)), Index_addr(cache, 0), Range_length(range1) * size);
			return;
		}
	} else {
		if (Range_length(range2) <= cache_size) {
			memcpy(Index_addr(cache, 0), Index_addr(array, range2.start), Range_length(range2) * size);
			memmove(Index_addr(array, range2.end - Range_length(range1)), Index_addr(array, range1.start), Range_length(range1) * size);
			memcpy(Index_addr(array, range1.start), Index_addr(cache, 0), Range_length(range2) * size);
			return;
		}
	}

	Reverse(array, range1, size);
	Reverse(array, range2, size);
	Reverse(array, range, size);
}

/* calculate how to scale the index value to the range within the array */
/* the bottom-up merge sort only operates on values that are powers of two, */
/* so scale down to that power of two, then use a fraction to scale back again */
typedef struct {
	size_t size, power_of_two;
	size_t numerator, decimal;
	size_t denominator, decimal_step, numerator_step;
} WikiIterator;

void WikiIterator_begin(WikiIterator *me) {
	me->numerator = me->decimal = 0;
}

Range WikiIterator_nextRange(WikiIterator *me) {
	size_t start = me->decimal;

	me->decimal += me->decimal_step;
	me->numerator += me->numerator_step;
	if (me->numerator >= me->denominator) {
		me->numerator -= me->denominator;
		me->decimal++;
	}

	return Range_new(start, me->decimal);
}

bool WikiIterator_finished(WikiIterator *me) {
	return (me->decimal >= me->size);
}

bool WikiIterator_nextLevel(WikiIterator *me) {
	me->decimal_step += me->decimal_step;
	me->numerator_step += me->numerator_step;
	if (me->numerator_step >= me->denominator) {
		me->numerator_step -= me->denominator;
		me->decimal_step++;
	}

	return (me->decimal_step < me->size);
}

size_t WikiIterator_length(WikiIterator *me) {
	return me->decimal_step;
}

WikiIterator WikiIterator_new(size_t size2, size_t min_level) {
	WikiIterator me;
	me.size = size2;
	me.power_of_two = FloorPowerOfTwo(me.size);
	me.denominator = me.power_of_two/min_level;
	me.numerator_step = me.size % me.denominator;
	me.decimal_step = me.size/me.denominator;
	WikiIterator_begin(&me);
	return me;
}

/* merge two ranges from one array and save the results into a different array */
void MergeInto(void *arr_from, const Range A, const Range B, size_t size, const Comparison compare, void *arr_into) {
	char *from = (char *)arr_from;
	char *into = (char *)arr_into;
	char *A_index = Index_addr(from, A.start), *B_index = Index_addr(from, B.start);
	char *A_last = Index_addr(from, A.end), *B_last = Index_addr(from, B.end);
	char *insert_index = Index_addr(into, 0);

	while (true) {
		if (!COMPARE(B_index, A_index)) {
			for (size_t i = 0; i < size; i++)
				 insert_index[i] = A_index[i];
			A_index += size;
			insert_index += size;
			if (A_index == A_last) {
				/* copy the remainder of B into the final array */
				memcpy(insert_index, B_index, (B_last - B_index) * size);
				break;
			}
		} else {
			for (size_t j = 0; j < size; j++)
				 insert_index[j] = B_index[j];
			B_index += size;
			insert_index += size;
			if (B_index == B_last) {
				/* copy the remainder of A into the final array */
				memcpy(insert_index, A_index, (A_last - A_index) * size);
				break;
			}
		}
	}
}

/* merge operation using an external buffer, */
void MergeExternal(void *arr, const Range A, const Range B, size_t size, const Comparison compare, void *cache_base) {
	char *array = (char *)arr;
	char *cache = (char *)cache_base;
	/* A fits into the cache, so use that instead of the internal buffer */
	char *A_index = Index_addr(cache, 0);
	char *B_index = Index_addr(array, B.start);
	char *insert_index = Index_addr(array, A.start);
	char *A_last = Index_addr(cache, Range_length(A));
	char *B_last = Index_addr(array, B.end);

	if (Range_length(B) > 0 && Range_length(A) > 0) {
		while (true) {
			if (!COMPARE(B_index, A_index)) {
				for (size_t i = 0; i < size; i++)
					insert_index[i] = A_index[i];
				A_index += size;
				insert_index += size;
				if (A_index == A_last) break;
			} else {
				for (size_t j = 0; j < size; j++)
					insert_index[j] = B_index[j];
				B_index += size;
				insert_index += size;
				if (B_index == B_last) break;
			}
		}
	}

	/* copy the remainder of A into the final array */
	memcpy(insert_index, A_index, (A_last - A_index) * size);
}

/* merge operation using an internal buffer */
void MergeInternal(void *arr, const Range A, const Range B, size_t size, const Comparison compare, const Range buffer) {
	char *array = (char *)arr;
	/* whenever we find a value to add to the final array, swap it with the value that's already in that spot */
	/* when this algorithm is finished, 'buffer' will contain its original contents, but in a different order */
	size_t A_count = 0, B_count = 0, insert = 0;

	if (Range_length(B) > 0 && Range_length(A) > 0) {
		while (true) {
			if (!COMPARE(Index_addr(array, B.start + B_count), Index_addr(array, buffer.start + A_count))) {
				Swap(Index_addr(array, A.start + insert), Index_addr(array, buffer.start + A_count), size);
				A_count++;
				insert++;
				if (A_count >= Range_length(A)) break;
			} else {
				Swap(Index_addr(array, A.start + insert), Index_addr(array, B.start + B_count), size);
				B_count++;
				insert++;
				if (B_count >= Range_length(B)) break;
			}
		}
	}

	/* swap the remainder of A into the final array */
	BlockSwap(array, buffer.start + A_count, A.start + insert, Range_length(A) - A_count, size);
}

/* merge operation without a buffer */
void MergeInPlace(void *arr, Range A, Range B, size_t size, const Comparison compare, void *cache_base, const size_t cache_size) {
	char *array = (char *)arr;
	char *cache = (char *)cache_base;
	if (Range_length(A) == 0 || Range_length(B) == 0) return;

	/*
	 this just repeatedly binary searches into B and rotates A into position.
	 the paper suggests using the 'rotation-based Hwang and Lin algorithm' here,
	 but I decided to stick with this because it had better situational performance

	 (Hwang and Lin is designed for merging subarrays of very different sizes,
	 but WikiSort almost always uses subarrays that are roughly the same size)

	 normally this is incredibly suboptimal, but this function is only called
	 when none of the A or B blocks in any subarray contained 2√A unique values,
	 which places a hard limit on the number of times this will ACTUALLY need
	 to binary search and rotate.

	 according to my analysis the worst case is √A rotations performed on √A items
	 once the constant factors are removed, which ends up being O(n)

	 again, this is NOT a general-purpose solution – it only works well in this case!
	 kind of like how the O(n^2) insertion sort is used in some places
	 */

	while (true) {
		/* find the first place in B where the first item in A needs to be inserted */
		size_t mid = BinaryFirst(array, Index_addr(array, A.start), B, size, compare);

		/* rotate A into place */
		size_t amount = mid - A.end;
		Rotate(array, Range_length(A), Range_new(A.start, mid), cache, cache_size, size);
		if (B.end == mid) break;

		/* calculate the new A and B ranges */
		B.start = mid;
		A = Range_new(A.start + amount, B.start);
		A.start = BinaryLast(array, Index_addr(array, A.start), A, size, compare);
		if (Range_length(A) == 0) break;
	}
}

