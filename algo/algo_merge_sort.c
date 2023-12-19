#include "pch.h"
#include "common.h"
#include "algo_merge_sort.h"

static algo_errno_t merge_sort_impl(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *counter) {
    if (count <= 1) {
        return OK;
    }

    uint8_t *arr_ = arr;

    size_t mid = count / 2;
    ALGO_ERRNO_UNWRAP(merge_sort_impl(arr, mid, size, comp, threshold, counter));
    ALGO_ERRNO_UNWRAP(merge_sort_impl((uint8_t *)arr + mid * size, count - mid, size, comp, threshold, counter));

    uint8_t *tmp = (uint8_t *)malloc(count * size * sizeof(uint8_t));
    PTR_ASSERT_NONNULL(tmp);

    size_t i = 0;
    size_t j = mid;
    size_t k = 0;
    while (i < mid && j < count) {
        COUNTER_INC_AND_CHECK_FREE(*counter, threshold, tmp);
        if (comp(arr_ + i * size, arr_ + j * size) <= 0) {
            memcpy(tmp + (k++) * size, arr_ + (i++) * size, size);
        } else {
            memcpy(tmp + (k++) * size, arr_ + (j++) * size, size);
        }
    }

    while (i < mid) {
        memcpy(tmp + (k++) * size, arr_ + (i++) * size, size);
    }
    while (j < count) {
        memcpy(tmp + (k++) * size, arr_ + (j++) * size, size);
    }

    memcpy(arr_, tmp, count * size);
    free(tmp);

    return OK;
}

algo_errno_t merge_sort(void *arr, size_t count, size_t size, comp_t comp, uint64_t threshold, uint64_t *out_counter) {
    return merge_sort_impl(arr, count, size, comp, threshold, out_counter);
}
