#include "pch.h"
#include "common.h"
#include "algo_stdlib_qsort.h"

algo_errno_t stdlib_qsort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    qsort(arr, count, size, comp);

    return OK;
}
