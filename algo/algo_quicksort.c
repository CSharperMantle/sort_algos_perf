#include "pch.h"
#include "common.h"
#include "algo_quicksort.h"

static algo_errno_t partition(void *arr, size_t size, size_t low, size_t high, comp_t comp, uint64_t threshold, uint64_t *counter, size_t *out_result) {
    uint8_t *arr_ = arr;

    uint8_t *pivot = arr_ + high * size;
    size_t i = low - 1;

    for (size_t j = low; j <= high; j++) {
        COUNTER_INC_AND_CHECK(*counter, threshold);
        if (comp(arr_ + j * size, pivot) < 0) {
            i++;
            swap_bytes(arr_ + i * size, arr_ + j * size, size);
        }
    }
    swap_bytes(arr_ + (i + 1) * size, arr_ + high * size, size);
    *out_result = i + 1;

    return OK;
}

static algo_errno_t quicksort_impl(void *arr, size_t size, size_t low, size_t high, comp_t comp, uint64_t threshold, uint64_t *counter) {
    if ((low - high) & (1ull << (sizeof(size_t) * 8 - 1))) {
        size_t p = 0;
        ALGO_ERRNO_UNWRAP(partition(arr, size, low, high, comp, threshold, counter, &p));
        ALGO_ERRNO_UNWRAP(quicksort_impl(arr, size, low, p - 1, comp, threshold, counter));
        ALGO_ERRNO_UNWRAP(quicksort_impl(arr, size, p + 1, high, comp, threshold, counter));
    }
    return OK;
}

algo_errno_t quicksort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    if (count <= 1) {
        return OK;
    }

    return quicksort_impl(arr, size, 0, count - 1, comp, threshold, out_counter);
}

