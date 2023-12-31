#include "pch.h"
#include "algo.h"

const named_algo_t ALGOS[] = {
	{ .algo = bubblesort, .name = "bubblesort" },
	{ .algo = insertion_sort, .name = "insertion" },
	{ .algo = merge_sort, .name = "merge sort" },
	{ .algo = quicksort_highpivot, .name = "high pivot quicksort" },
	{ .algo = selection_sort, .name = "selection" },
	{ .algo = stdlib_qsort, .name = "stdlib qsort" },
	{ .algo = bogosort_determ, .name = "deterministic bogosort" },
	{ .algo = eradix_sort, .name = "extended radix sort" },
	{ .algo = NULL, .name = NULL }
};
