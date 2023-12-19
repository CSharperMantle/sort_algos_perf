#pragma once

#ifndef ALGO_H_INCLUDED_
#define ALGO_H_INCLUDED_

#include "common.h"
#include "algo_bubblesort.h"
#include "algo_insertion_sort.h"
#include "algo_merge_sort.h"
#include "algo_quicksort.h"
#include "algo_selection_sort.h"
#include "algo_stdlib_qsort.h"
#include "algo_bogosort.h"

typedef struct named_algo_ {
	const algo_t algo;
	const char *name;
} named_algo_t;

extern const named_algo_t ALGOS[];

#endif // ALGO_H_INCLUDED_
