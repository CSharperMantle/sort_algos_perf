#include "pch.h"
#include "common.h"
#include "algo_insertion_sort.h"

static int comp_proxy(comp_t comp, const void *a, const void *b, uint64_t *counter) {
    (*counter)++;
    return comp(a, b);
}

algo_errno_t insertion_sort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    uint8_t *arr_ = arr;

    for (size_t i = 1; i < count; i++) {
        size_t j = i;
        while (j > 0 && comp_proxy(comp, arr_ + j * size, arr_ + (j - 1) * size, out_counter) < 0) {
            COUNTER_CHECK(*out_counter, threshold);
            swap_bytes(arr_ + j * size, arr_ + (j - 1) * size, size);
            j--;
        }
    }

    return OK;
}
