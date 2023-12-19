#pragma once

#ifndef ALGO_INSERTION_SORT_H_INCLUDED_
#define ALGO_INSERTION_SORT_H_INCLUDED_

#include "common.h"

algo_errno_t insertion_sort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter);

#endif // ALGO_INSERTION_SORT_H_INCLUDED_
