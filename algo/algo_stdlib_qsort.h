#pragma once

#ifndef ALGO_STDLIB_QSORT_H_INCLUDED_
#define ALGO_STDLIB_QSORT_H_INCLUDED_

#include "common.h"

algo_errno_t stdlib_qsort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter);

#endif // ALGO_STDLIB_QSORT_H_INCLUDED_
